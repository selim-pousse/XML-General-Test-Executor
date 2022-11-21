#include <Agte.h>
#include <QTextStream>
#include <QCommandLineParser>
#include <BteXmlParser.h>
#include <QSettings>
#include <QDebug>
#include <iostream> // for std::cin.ignore()

const QString CAgte::k_strAppName  = "ANNAX General Test Executor";

CAgte::CAgte(QObject *parent) :
    QObject(parent),
    m_bShowLog(false),
    m_xCin(stdin),
    m_xCout(stdout)
{
    connect(&CBteRunner::getInstance(),SIGNAL(appendLogText(QString,uint)),this,SLOT(onAppendLogText(QString,uint)));
    connect(&CBteRunner::getInstance(),SIGNAL(bteMessage(uint,QString,QString,uint,QString)),this,SLOT(onBteMessage(uint,QString,QString,uint,QString)));
    connect(&CBteRunner::getInstance(),SIGNAL(testProcedureDone(bool)),this,SLOT(onTestProcedureDone(bool)));

    initialize();
}


void CAgte::initialize()
{
    QCoreApplication::setApplicationName(k_strAppName);

    QString strFileName = CBteXmlParser::getInstance().directoryOf("configfiles").path() + "/sysConfig.ini";
    QSettings xSetting(strFileName,QSettings::IniFormat);
    QString strAppVersion   = xSetting.value("AnnaxGTE.version","Unknown").toString();

    QCoreApplication::setApplicationVersion(strAppVersion);

    QCommandLineParser xParser;
    xParser.setApplicationDescription("AnnaxGTE is a generic tool which performs a set of actions described in a XML file representing the Test procedure.");
    xParser.addHelpOption();
    xParser.addVersionOption();
    xParser.addPositionalArgument("source", tr("Source XML file to execute."));

    // A boolean option with a single name (-p)
    QCommandLineOption showProgressOption("p", tr("Show progress during execution"));
    xParser.addOption(showProgressOption);

    // A string option with multiple names (-m, --mask)
    QCommandLineOption maskOption(QStringList() << "m" << "mask",
           tr("Set mask for filtering the messages (default value = 0xffffffff)."),
           tr("mask")                      );
    xParser.addOption(maskOption);

    // An option with a value (-f, --filter)
    QCommandLineOption filterOption(QStringList() << "f" << "filter",
           tr("Filter with a list of TestCase Ids."),
           tr("id1[,id2..]")                      );
    xParser.addOption(filterOption);

    // An option with a value
    QCommandLineOption reportFileOption(QStringList() << "o" << "output",
           tr("Destination of the report file."),
           tr("reportFile"));
    xParser.addOption(reportFileOption);

    // Process the actual command line arguments given by the user
    xParser.process(*QCoreApplication::instance());

    const QStringList args = xParser.positionalArguments();
    // source is args.at(0)

    if (args.size()==0)
    {
        xParser.showHelp();
    }
    else
    {
        m_bShowLog = xParser.isSet(showProgressOption);
        QString strTargetFile = xParser.value(reportFileOption);
        const QString& strMask = xParser.value(maskOption);
        const QString& strFilter = xParser.value(filterOption);
        const QString& strSourceFile = args.at(0);

        if (strTargetFile.isEmpty())
        {
            strTargetFile = QCoreApplication::applicationDirPath() + "/" +
                            QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + "_" +
                            QFileInfo(strSourceFile).fileName().replace(QRegExp("(\\.\\w+)$"),".log");
        }

        uint uiLogMask  = 0xffffffff;
        if (!strMask.isEmpty())
        {
            bool bOk;
            uint uiTmp;
            if (strMask.left(2)=="0x")
            {
                uiTmp = strMask.toUInt(&bOk,16);
            }
            else
            {
                uiTmp = strMask.toUInt(&bOk);
            }
            if (bOk)
            {
                uiLogMask = uiTmp;
            }
        }

        qDebug()    << "bShowProgress=" << m_bShowLog       << endl
                    << "strTargetFile=" << strTargetFile    << endl
                    << "strMask="       << uiLogMask        << endl
                    << "source="        << strSourceFile    << endl
                    << "filter="        << strFilter        << endl;

         CBteRunner::getInstance().setLogMask(uiLogMask);

         if(CBteRunner::getInstance().loadXmlFile(strSourceFile))
         {
             m_xLogger.setLogger(k_strAppName + strAppVersion, strTargetFile);
             bool bAll = strFilter.isEmpty();
             CBteRunner::getInstance().startTestProcedure(bAll,strFilter.split(","));
         }
    }
}

/** function for tracing and logging */
void CAgte::onAppendLogText(QString strMsg, uint uiMask)
{
    qDebug() << __FUNCTION__;
    //         datetime                  Error TypeErr           ipDevice Device                TCaseId StepId            MessageText
    //strLineLog = "%1" + k_strDelimiter + "%2 %3" + k_strDelimiter + "%4 %5" + k_strDelimiter + "%6 %7" + k_strDelimiter + "%8";

    if (m_bShowLog || (uiMask & CBteRunner::eERRT_PARSING) )
    {
        QStringList lstMsg = strMsg.split(CBteRunner::k_strDelimiter);
        QStringList lstFullMsg;
        QStringList::iterator it = lstMsg.begin();
        while (it != lstMsg.end())
        {
            if (it+1 == lstMsg.end())
            {
                lstFullMsg << (*it);
            }
            else
            {
                lstFullMsg << (*it).split(" ");
            }
            ++it;
        }

        QString strMsg = QString(lstFullMsg.at(8)).replace("\\n","\n");

        if (uiMask & CBteRunner::eERRT_PARSING)
        {
            m_xCout << lstFullMsg.at(2) << " "  // error
                    << strMsg << endl; // Message
            exit(EXIT_FAILURE);
        }
        else
        {
            m_xCout << lstFullMsg.at(2) << " "  // error
                    << lstFullMsg.at(1) << " " // time
                    << lstFullMsg.at(5) << " " // device
                    << lstFullMsg.at(6) << "/" << lstFullMsg.at(7) << " " // TCase + Step
                    << strMsg << endl; // Message
        }
    }

    if (m_xLogger.isOpen())
    {
        m_xLogger.reportMessage(strMsg);
    }
}

/** Message sent by BTE to transform it into a interactive grapical component **/
void CAgte::onBteMessage(uint uiIdReq, QString strTitle, QString strMsg, uint eMsg, QString strVariable)
{
    qDebug() << __FUNCTION__ << strTitle << " " << strVariable;
    QString strRes("n");
    m_xCout << strMsg;
    m_xCout.flush();
    switch (eMsg)
    {
        case CBteRunner::eMSGT_CONFIRM_V1:
        {
            m_xCout << "\n" << "Press <Enter> to continue..";
            m_xCout.flush();
            std::cin.ignore();
            CBteRunner::getInstance().confirmRequestResponse(uiIdReq, QVariant(true));
            break;
        }
        case CBteRunner::eMSGT_CONFIRM_V2:
        {
            m_xCout << "\n" << "[y/n]? ";
            m_xCout.flush();
            m_xCin >> strRes;
            CBteRunner::getInstance().confirmRequestResponse(uiIdReq, QVariant(strRes == "y") );
            break;
        }
        case CBteRunner::eMSGT_CONFIRM_V3:
        {
            m_xCout << "\n" << "[y/n/N]? ";
            m_xCout.flush();
            m_xCin >> strRes;
            CBteRunner::getInstance().confirmRequestResponse(uiIdReq, QVariant( strRes == "y"? 1: (strRes == "n" ? 0:-1) ));
            break;
        }
        case CBteRunner::eMSGT_INPUT:
        {
            m_xCout << endl;
            m_xCin >> strRes;
            CBteRunner::getInstance().confirmRequestResponse(uiIdReq, QVariant(strRes));
            break;
        }
        case CBteRunner::eMSGT_INFO:
        default:
            m_xCout << endl;
            break;
    }
}


/** the test Procedure requested is done **/
void CAgte::onTestProcedureDone(bool bSuccess)
{
    qDebug() << __FUNCTION__;
    const QString& strMsg = bSuccess? tr("SUCCESS: The Test procedure is sucessfully done!"): tr("FAILURE: The Test Procedure has failed!");
    m_xCout << strMsg << endl;

    if (bSuccess)
    {
        exit(EXIT_SUCCESS);
    }
    else
    {
        exit(EXIT_FAILURE);
    }
}
