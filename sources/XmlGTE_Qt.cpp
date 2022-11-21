//=================================================================================================
// Copyright (C) 2016 Pousse Systems
//
// Name           CSelectTestcaseDlg
//
// Description    Main Window of the tool.
//
// Author         Selim Pousse
//=================================================================================================
#include "XmlGTE_Qt.h"
#include "ui_XmlGTE.h"
#include "ui_SelectTestcaseDlg.h"
#include "ui_AboutXmlGTE.h"
#include <QRegExp>
#include <QException>
#include <QMessageBox>
#include <QFile>
#include <QDateTime>
#include <QStringList>
#include <QFileDialog>
#include <QInputDialog>
#include <BteRunner.h>
#include <BteXmlParser.h>
#include <QApplication>
#include <QScrollBar>
#include <QSettings>
#include <QRadioButton>
#include <QWhatsThis>
#include <QVBoxLayout>
#include <QStandardPaths>


const QString CXmlGTE::k_strAppName         = "XML General Test Executor (XmlGTE)";
const QString CXmlGTE::k_strKeyLogMask      = "log_mask";

CXmlGTE::CXmlGTE(QString strAppVersion, QWidget *parent) :
    QMainWindow(parent),
    m_pxUi(new Ui::CXmlGTE),
    m_strCurrentFile(""),
    m_uiBteMsgReqid(0),
    m_xTabViewModel(0,5),
    m_xFilterProxyModel(),
    m_pxCurrentItemTCase(NULL),
    k_strAppVersion(strAppVersion)
{
    m_pxUi->setupUi(this);
    m_xGifAnim.setFileName(":/images/loader.gif");
    m_xGifAnim.start();

    connect(&m_xGifAnim, SIGNAL(frameChanged(int)), this, SLOT(onUpdateFrameSlider()));

    QApplication::setApplicationName(k_strAppName);
    QApplication::setOrganizationName("Pousse-Systems");
    QApplication::setOrganizationDomain("pousse.systems");
    this->setWindowTitle(k_strAppName);

    m_xSpacerWidg.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_pxUi->mainToolBar->addWidget(&m_xSpacerWidg); // add the spacer before the label
    m_pxUi->mainToolBar->addWidget(&m_xAnnaxWidg);// add the campagny label

    m_xLogger.setLogger(k_strAppName +" (v" +  k_strAppVersion + ")",
                          QApplication::applicationDirPath(),
                          QString("XmlGTE.log"));

    connect(&CBteRunner::getInstance(),SIGNAL(appendLogText(QString,uint)),this,SLOT(onAppendLogText(QString,uint)));
    connect(&CBteRunner::getInstance(),SIGNAL(bteMessage(uint,QString,QString,uint,QString)),this,SLOT(onBteMessageNoModal(uint,QString,QString,uint,QString)));
    connect(&CBteRunner::getInstance(),SIGNAL(testProcedureDone(bool)),this,SLOT(onTestProcedureDone(bool)));
    connect(&CBteRunner::getInstance(),SIGNAL(testCaseStarted(QString,QString)),this,SLOT(onTestCaseStarted(QString,QString)));
    connect(&CBteRunner::getInstance(),SIGNAL(testCaseDone(QString,QString,bool,QString)),this,SLOT(onTestCaseDone(QString,QString,bool,QString)));
    connect(&CBteRunner::getInstance(),SIGNAL(collectionOfDeviceEvaluated(QList<QStringList>)),this,SLOT(onCollectionOfDevice(QList<QStringList>)));

    connect(&CBteRunner::getInstance(),SIGNAL(notifyDetectionStatus(QString,bool)),this,SLOT(onNotifyDetectionStatus(QString,bool)));
    connect(&CBteRunner::getInstance(),SIGNAL(notifyVersions(QString,QStringList,QStringList)),this,SLOT(onNotifyDeviceVersions(QString,QStringList,QStringList)));


    CBteRunner::getInstance().setLogMask(0xffffffff);

    m_xTabViewModel.setColumnCount(6);
    m_xTabViewModel.setHeaderData(0, Qt::Horizontal, tr(""));
    m_xTabViewModel.setHeaderData(1, Qt::Horizontal, tr("Level"));
    m_xTabViewModel.setHeaderData(2, Qt::Horizontal, tr("Time"));
    m_xTabViewModel.setHeaderData(3, Qt::Horizontal, tr("Device"));
    m_xTabViewModel.setHeaderData(4, Qt::Horizontal, tr("Testcase/Step"));
    m_xTabViewModel.setHeaderData(5, Qt::Horizontal, tr("Message"));
    m_xTabViewModel.setRowCount(0);

    m_xFilterProxyModel.setSourceModel(&m_xTabViewModel);

    m_xFilterProxyModel.setFilterKeyColumns(QList<qint32>() << 0 << 1 << 3 << 4 << 5);
    m_pxUi->tabViewContent->setModel(&m_xFilterProxyModel);


    QIcon xIcon;
    xIcon.addFile(":/images/check.png",QSize(),QIcon::Normal,QIcon::On);
    xIcon.addFile(":/images/empty.png",QSize(),QIcon::Normal,QIcon::Off);
    m_pxUi->btnFail->setIcon(xIcon);
    m_pxUi->btnPass->setIcon(xIcon);
    m_pxUi->btnSkip->setIcon(xIcon);
    m_pxUi->btnErro->setIcon(xIcon);
    m_pxUi->btnWarn->setIcon(xIcon);
    m_pxUi->btnInfo->setIcon(xIcon);
    m_pxUi->btnMsg1->setIcon(xIcon);
    m_pxUi->btnMsg2->setIcon(xIcon);

    QVBoxLayout* vbox = new QVBoxLayout();
    m_pxUi->menuViewMode->setLayout(vbox);
    QRadioButton * xUserMode = new QRadioButton(tr("User Mode"),m_pxUi->menuViewMode);
    QRadioButton * xExpertMode = new QRadioButton(tr("Expert Mode"),m_pxUi->menuViewMode);
    vbox->addWidget(xUserMode);
    vbox->addWidget(xExpertMode);
    QObject::connect(xUserMode,SIGNAL(toggled(bool)),this,SLOT(onViewModeChanged(bool)));
    xExpertMode->setChecked(true);

    //m_pxUi->tabViewContent->setItemDelegateForColumn(0,&m_xBteTabViewDelegate);

    CXmlGTE::showFullScreen();
    CXmlGTE::showMaximized();

    restoreMask();

}

/** restore the mask **/
void CXmlGTE::restoreMask()
{
    // extract the log mask and restore the components' state concerned
    QSettings xSettings;
    if (xSettings.value(k_strKeyLogMask).toString().isEmpty())
    {
        xSettings.setValue(k_strKeyLogMask,0xffff0002);
    }
    uint uiLogMask = xSettings.value(k_strKeyLogMask).toUInt();
    uint uiLogLevel = uiLogMask & 0x000000ff;

    m_pxUi->slidLogLevel->setValue(uiLogLevel);
    m_pxUi->btnFail->setChecked( (uiLogMask & CBteRunner::eLOGL_FAIL) );
    m_pxUi->btnPass->setChecked( (uiLogMask & CBteRunner::eLOGL_PASS) );
    m_pxUi->btnSkip->setChecked( (uiLogMask & CBteRunner::eLOGL_SKIP) );
    m_pxUi->btnErro->setChecked( (uiLogMask & CBteRunner::eLOGL_ERRO) );
    m_pxUi->btnWarn->setChecked( (uiLogMask & CBteRunner::eLOGL_WARN) );
    m_pxUi->btnInfo->setChecked( (uiLogMask & CBteRunner::eLOGL_INFO) );
    m_pxUi->btnMsg1->setChecked( (uiLogMask & CBteRunner::eLOGL_MESG) );
    m_pxUi->btnMsg2->setChecked( (uiLogMask & CBteRunner::eLOGL_TRAC) );
    // update the filters for the buttons 'sevrity'
    setFilterMsgType();
}

CXmlGTE::~CXmlGTE()
{
    //save personal data
    QSettings xSettings;
    uint uiMask = 0xffff0000 + m_pxUi->slidLogLevel->value();
    if (m_pxUi->btnFail->isChecked()) {uiMask |=  CBteRunner::eLOGL_FAIL;}
    if (m_pxUi->btnPass->isChecked()) {uiMask |=  CBteRunner::eLOGL_PASS;}
    if (m_pxUi->btnSkip->isChecked()) {uiMask |=  CBteRunner::eLOGL_SKIP;}
    if (m_pxUi->btnErro->isChecked()) {uiMask |=  CBteRunner::eLOGL_ERRO;}
    if (m_pxUi->btnWarn->isChecked()) {uiMask |=  CBteRunner::eLOGL_WARN;}
    if (m_pxUi->btnInfo->isChecked()) {uiMask |=  CBteRunner::eLOGL_INFO;}
    if (m_pxUi->btnMsg1->isChecked()) {uiMask |=  CBteRunner::eLOGL_MESG;}
    if (m_pxUi->btnMsg2->isChecked()) {uiMask |=  CBteRunner::eLOGL_TRAC;}
    xSettings.setValue(k_strKeyLogMask,uiMask);

    if ( m_pxUi->menuViewMode->layout() != NULL )
    {
        QLayoutItem* pItem;
        while ( ( pItem = m_pxUi->menuViewMode->layout()->takeAt( 0 ) ) != NULL )
        {
            delete pItem->widget();
            delete pItem;
        }
        delete m_pxUi->menuViewMode->layout();
    }

    delete m_pxUi;
}


/** initialize the view mode as user (restricted)  **/
void CXmlGTE::initUserMode()
{
    QSettings xSettings;
    xSettings.setValue(k_strKeyLogMask,0xffff0002);
    restoreMask();
    m_pxUi->menuSettings->menuAction()->setVisible(false);
    setUserMode(true);
}

/** initialize the programm as a plugin for the given device **/
void CXmlGTE::initPluginForDevice(const QString& strXmlFile)
{
    qDebug() << __FUNCTION__ << strXmlFile;

    // set the view to the user mode
    ((QRadioButton*)m_pxUi->menuViewMode->layout()->takeAt(0)->widget())->setChecked(true);

    m_pxUi->actionOpen->setVisible(false);
    m_pxUi->actionCleanAll->setVisible(false);

    if(!strXmlFile.isEmpty() && CBteRunner::getInstance().loadXmlFile(strXmlFile))
    {
        this->setWindowTitle(k_strAppName + " - " + CBteXmlParser::getInstance().getDescription());
        m_strCurrentFile = CBteXmlParser::getInstance().getXmlFilePath();
        readyToStartTestProcedure();
    }
    else
    {
        QMessageBox::critical(this,
         tr("Error critical"),
         tr("Unable to load the Test Procedure descripted in the file %1!").arg(strXmlFile),
         QMessageBox::Ok);
    }
}

void CXmlGTE::on_actionOpen_triggered()
{
    static const QString strKeyDir("default_dir");

    QSettings xSettings;
    if (xSettings.value(strKeyDir).toString().isEmpty())
    {
        if (QDir("/opt/XmlGTE/TestCases").exists())
        {
            xSettings.setValue(strKeyDir, "/opt/XmlGTE/TestCases");
        }
        else
        {
            xSettings.setValue(strKeyDir,QCoreApplication::applicationDirPath());
        }
    }

    QString strOpenFileName = QFileDialog::getOpenFileName(this, tr("Open Test Procedure File"),xSettings.value(strKeyDir).toString(),tr("Xml Test Procedure")+ "(*.xml)");
    if(!strOpenFileName.isEmpty())
    {
        QDir xCurrentDir;
        xSettings.setValue(strKeyDir, xCurrentDir.absoluteFilePath(strOpenFileName));

        on_actionCleanAll_triggered();

        if(CBteRunner::getInstance().loadXmlFile(strOpenFileName))
        {
            this->setWindowTitle(k_strAppName + " - " + CBteXmlParser::getInstance().getDescription());
            m_strCurrentFile = strOpenFileName;
            readyToStartTestProcedure();
        }
        else
        {
            QMessageBox::critical(this,
             tr("Error critical"),
             tr("Unable to load the Test Procedure descripted in the file '%1'!").arg(strOpenFileName),
             QMessageBox::Ok);
        }
    }
}


void CXmlGTE::on_actionPlay_triggered()
{
    if(CBteRunner::getInstance().loadXmlFile(m_strCurrentFile)==1)
    {
        const QList<QStringList>& xListTC = CBteXmlParser::getInstance().getTestCases();
        QStandardItem* pxItem;
        bool bSame = true;
        foreach(pxItem,m_lstxItemTCase)
        {
            bSame = false;
            foreach(QStringList xElem,xListTC)
            {
                const QString& id   = pxItem->data().toString() ;
                const QString& desc = pxItem->data(Qt::UserRole+3).toString();
                bSame = (xElem.at(0) == id &&
                         xElem.at(1) == desc);
                if (bSame) break;
            }

            if (!bSame)
            {
                cleanTCases();
                initTestCases();
                break;
            }
        }
    }

    cleanContents();

    m_pxUi->actionStop->setEnabled(true);
    m_pxUi->actionPlay->setEnabled(false);

    startTestProcedure();
}


void CXmlGTE::on_actionStop_triggered()
{
    CBteRunner::getInstance().forceToStop();
    /*CBteRunner::getInstance().disconnectToDevices();
    const QString& strMsg  =   "******************** " + tr("Test Procedure stopped manually") + " ********************";
    CBteRunner::getInstance().sendLogText(strMsg,CBteRunner::eLOGL_INFO);*/

}


void CXmlGTE::on_actionExit_triggered()
{
    close();
}



/** when a new device's informations has been collected
 **  it reinitialize the components aimed to detect the devices' status **/
void CXmlGTE::onCollectionOfDevice(QList<QStringList> xDeviceContainer)
{
    qDebug() << __FUNCTION__;
    // delete the previous actions
    foreach(QAction * xAct,m_pxUi->menuStatus->actions())
    {
        if (xAct)
        {
            xAct->deleteLater();
        }
    }
    m_pxUi->menuStatus->clear();

    foreach (const QStringList& xlistDev,xDeviceContainer)
    {
        if (xlistDev.at(2).compare("required",Qt::CaseInsensitive)==0)
        {
            QAction * pxAct = m_pxUi->menuStatus->addAction(QIcon(":/images/red16.png"),xlistDev.at(0),this,SLOT(onRequestVersion()));
            pxAct->setData(false); // false beacuse not detected
            pxAct->setWhatsThis(tr("No Info gotten right now.."));
        }
    }
}

void CXmlGTE::readyToStartTestProcedure()
{   
    m_pxUi->actionStop->setEnabled(false);
    m_pxUi->actionPlay->setEnabled(true);

    cleanContents();
    cleanTCases();
    initTestCases();

    this->statusBar()->showMessage(tr("Click the 'Start' Icon to run the Test procedure."));
}

void CXmlGTE::startTestProcedure()
{
    for( int i=0; i < m_lstxItemTCase.size() ;++i)
    {
        m_lstxItemTCase.at(i)->setData(QIcon(":/images/arrow16.png"),Qt::DecorationRole);
        m_lstxItemTCase.at(i)->setData(m_lstxItemTCase.at(i)->data(),Qt::ToolTipRole);
    }

    if(m_pxUi->lstviewTestCases->model()->rowCount())
    {
        QStringList lststrTCs;
        for (int i=0;i < m_pxUi->lstviewTestCases->model()->rowCount();i++)
        {
            lststrTCs << m_pxUi->lstviewTestCases->model()->data(m_pxUi->lstviewTestCases->model()->index(i,0),Qt::ToolTipRole).toString();
        }

        bool bAll = (m_pxUi->lstviewTestCases->model()->rowCount() == m_lstxItemTCase.size());
        CBteRunner::getInstance().startTestProcedure(bAll,lststrTCs);
    }
}


/** add new row into the tableView
 * @param strList list of values for each column
 */
void CXmlGTE::appendTabView(const QStringList & qstrList)
{
    this->statusBar()->setStyleSheet("color:gray");

    QColor xColorType;
    QString strSolveTyp  = "MSG";
    if (qstrList.at(2) == "FAIL")
    {
        this->statusBar()->setStyleSheet("color:red");
        xColorType = m_pxUi->btnFail->palette().button().color();
        strSolveTyp = "FAT";
    }
    else if (qstrList.at(2) == "PASS")
    {
        xColorType = m_pxUi->btnPass->palette().button().color();
        strSolveTyp = "COM";
    }
    else if (qstrList.at(2) == "SKIP")
    {
        xColorType = m_pxUi->btnSkip->palette().button().color();
        strSolveTyp = "WRN";
    }
    else if (qstrList.at(2) == "ERRO")
    {
        xColorType = m_pxUi->btnErro->palette().button().color();
        this->statusBar()->setStyleSheet("color:red");
        strSolveTyp = "ERR";
    }
    else if (qstrList.at(2) == "WARN")
    {
        this->statusBar()->setStyleSheet("color:orange");
        xColorType = m_pxUi->btnWarn->palette().button().color();
        strSolveTyp = "WRN";
    }
    else if (qstrList.at(2) == "INFO")
    {
        xColorType = m_pxUi->btnInfo->palette().button().color();
    }
    else if (qstrList.at(2) == "MESG")
    {
        xColorType = m_pxUi->btnMsg1->palette().button().color();
    }
    else
    {
        xColorType = m_pxUi->btnMsg2->palette().button().color();
    }

    QString strMsgSimple = QString(qstrList.at(8)).replace("\\n","\n").trimmed();
    QString strToolTip = QString(qstrList.at(8)).replace("\\n","\n").trimmed();;
    this->statusBar()->showMessage(strMsgSimple);

    bool bOk;
    uint uiLogLev = qstrList.at(3).toUInt(&bOk, 16)& 0x000000ff;

    QList<QStandardItem*> lstpxStdIt;
    lstpxStdIt  << new QStandardItem()
                << new QStandardItem(QString("Lev%1").arg(uiLogLev))
                << new QStandardItem(qstrList.at(1))
                << new QStandardItem(qstrList.at(5)=="NULL"?"Unknown":qstrList.at(5))
                << new QStandardItem( (qstrList.at(6)=="NULL"?"":qstrList.at(6))+"/"+(qstrList.at(7)=="Nope"?"":qstrList.at(7)))
                << new QStandardItem(strMsgSimple);

    lstpxStdIt.at(0)->setData(xColorType,Qt::BackgroundColorRole);
    lstpxStdIt.at(0)->setData(qstrList.at(2),Qt::ToolTipRole);
    lstpxStdIt.at(0)->setData(qstrList.at(2),Qt::UserRole+2);
    lstpxStdIt.at(0)->setData(strSolveTyp,Qt::UserRole+3);
    lstpxStdIt.at(1)->setData(uiLogLev,Qt::UserRole+2);
    lstpxStdIt.at(1)->setData(qstrList.at(3),Qt::UserRole+3);
    lstpxStdIt.at(2)->setData(qstrList.at(0),Qt::UserRole+2);
    lstpxStdIt.at(2)->setData(qstrList.at(1),Qt::UserRole+3);
    lstpxStdIt.at(3)->setData(qstrList.at(4),Qt::ToolTipRole);
    lstpxStdIt.at(3)->setData(qstrList.at(4),Qt::UserRole+2);
    lstpxStdIt.at(3)->setData(qstrList.at(5),Qt::UserRole+3);
    lstpxStdIt.at(4)->setData(qstrList.at(6),Qt::UserRole+2);
    lstpxStdIt.at(4)->setData(qstrList.at(7),Qt::UserRole+3);
    lstpxStdIt.at(5)->setData(strToolTip,Qt::ToolTipRole);
    QString strActionCmd = QString(qstrList.at(8)).replace("\\n","\n").split("\n").at(0);
    lstpxStdIt.at(5)->setData(strActionCmd, Qt::UserRole+2);

    //         datetime                  Error TypeErr           ipDevice Device                TCaseId StepId            MessageText
    //strLineLog = "%1" + k_strDelimiter + "%2 %3" + k_strDelimiter + "%4 %5" + k_strDelimiter + "%6 %7" + k_strDelimiter + "%8";

    m_xTabViewModel.appendRow(lstpxStdIt);

    m_pxUi->tabViewContent->resizeColumnsToContents();
    m_pxUi->tabViewContent->setColumnWidth(0,20);
    m_pxUi->tabViewContent->repaint();
    //m_pxUi->tabViewContent->setRowHeight(m_xTabViewModel.rowCount()-1,20);
    //m_pxUi->tabViewContent->setIndexWidget(m_xTabViewModel.index(m_xTabViewModel.rowCount()-1, 0), pxLabel);
    if(m_pxUi->ckboxAutoScroll->isChecked())
    {
      m_pxUi->tabViewContent->verticalScrollBar()->setValue(m_pxUi->tabViewContent->verticalScrollBar()->maximum());
    }
}



/** the test Procedure requested is done **/
void CXmlGTE::onTestProcedureDone(bool bSuccess)
{
    qDebug() << __FUNCTION__;

    if (m_pxCurrentItemTCase)
    {
        QStandardItem * xTmp = m_pxCurrentItemTCase;
        m_pxCurrentItemTCase = NULL;
        m_xGifAnim.stop();
        xTmp->setData(QIcon(":/images/arrow16.png"),Qt::DecorationRole);
    }

    const QString& strMsg = bSuccess? tr("The Test procedure is sucessfully done!"): tr("The Test Procedure has failed!");
    QMessageBox::information(this,
                             tr("Result of the test procedure:"),
                             strMsg,
                             QMessageBox::Ok);

    m_pxUi->actionStop->setEnabled(false);
    m_pxUi->actionPlay->setEnabled(true);
}

/** inform whenever a Testcase is started */
void CXmlGTE::onTestCaseStarted(QString strTestCaseId, QString strTxtTestCase)
{
    qDebug() << __FUNCTION__ << "::"<< strTxtTestCase;
    int iIndex = -1;
    m_pxCurrentItemTCase = getItemTCase(strTestCaseId, iIndex);
    if (m_pxCurrentItemTCase)
    {
        m_pxCurrentItemTCase->setData(QIcon(m_xGifAnim.currentPixmap()),Qt::DecorationRole);
    }
    m_xGifAnim.start();
}

/** inform whenever a Testcase is done */
void CXmlGTE::onTestCaseDone(QString strTestCaseId, QString strTxtTestCase, bool bSuccess, QString strReason)
{
    qDebug() << __FUNCTION__ << "::"<< strTxtTestCase;
    int iIndex = -1;
    QStandardItem* pxItem = getItemTCase(strTestCaseId, iIndex);
    if (pxItem == NULL) return;

    if (bSuccess)
    {
        pxItem->setData(QIcon(":/images/check16.png"),Qt::DecorationRole);
        pxItem->setData(strTestCaseId,Qt::ToolTipRole);
    }
    else
    {
        pxItem->setData(QIcon(":/images/cross16.png"),Qt::DecorationRole);
        pxItem->setData(strReason,Qt::ToolTipRole);

        m_pxUi->actionRepeat->setChecked(CBteRunner::getInstance().getRepeatFailure());
    }
    m_pxCurrentItemTCase = NULL;
    m_xGifAnim.stop();
}


/** return the associated Item of the given TestCase ID **/
QStandardItem* CXmlGTE::getItemTCase(const QString& strIdTestCase, int &iIndex)
{
    int i=0;
    for (i=0;i<m_lstxItemTCase.size();++i)
    {
        if(m_lstxItemTCase.at(i)->data() == strIdTestCase)
        {
           iIndex = i;
           return  m_lstxItemTCase.at(i);
        }
    }
    iIndex = -1;
    return NULL;
}

/** function for tracing and logging */
void CXmlGTE::onAppendLogText(QString strMsg, uint eDebugLev)
{
    qDebug() << __FUNCTION__ << eDebugLev << strMsg;

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

    appendTabView(lstFullMsg);

    if ((eDebugLev & m_strLogFileMask.toUInt(0,16)) == eDebugLev)
    {
        m_xLogger.reportMessage(strMsg);
    }
}


/** clean the models and contents of the TableView component **/
void CXmlGTE::cleanContents()
{
    for(int i=0;i<m_xTabViewModel.rowCount();++i)
    {
        if(m_xTabViewModel.item(i,0))
        {
            delete m_xTabViewModel.item(i,0);
            delete m_xTabViewModel.item(i,1);
            delete m_xTabViewModel.item(i,2);
            delete m_xTabViewModel.item(i,3);
            delete m_xTabViewModel.item(i,4);
        }
    }
    m_xTabViewModel.setRowCount(0);
}


/** clean the models and data related to the Testcases **/
void CXmlGTE::cleanTCases()
{
    QStandardItem* pxItem;
    foreach(pxItem,m_lstxItemTCase)
    {
        if(pxItem)
        {
           delete pxItem;
        }
    }
    m_lstxItemTCase.clear();

    // testcases
    if(m_pxUi->lstviewTestCases->model() != NULL)
    {
        delete m_pxUi->lstviewTestCases->model();
    }

    if(m_xRemoveTCaseDlg.m_pxUi->lstviewSelect->model() != NULL)
    {
        delete m_xRemoveTCaseDlg.m_pxUi->lstviewSelect->model();
    }

    if(m_xAddTCaseDlg.m_pxUi->lstviewSelect->model() != NULL)
    {
        delete m_xAddTCaseDlg.m_pxUi->lstviewSelect->model();
    }
}

/** initialization of the components for selecting, adding and removing the testcases */
void CXmlGTE::initTestCases()
{
    // init the test cases
    const QList<QStringList>& lstxTCases = CBteXmlParser::getInstance().getTestCases();
    QListIterator<QStringList> itmpTC(lstxTCases);

    m_pxUi->lstviewTestCases->setViewMode(QListView::ListMode);
    QStandardItemModel* pxStandardModel     = new QStandardItemModel(this);
    QStandardItemModel* pxModelForAdd       = new QStandardItemModel(this);

    QSortFilterProxyModel *pxModelForRemove = new QSortFilterProxyModel(this);
    //QStandardItemModel* pxModelForRemove    = new QStandardItemModel(this);

    while( itmpTC.hasNext() )
    {
        const QStringList& xTC = itmpTC.next();
        const bool bRequired = QVariant(xTC.at(2)=="required").toBool();
        QStandardItem* xItem = new QStandardItem(QIcon(":/images/arrow16.png"),xTC.at(1)+" ("+xTC.at(0)+")" + (bRequired?" - Required":""));
        xItem->setData(xTC.at(0)); // store the Test case ID.
        xItem->setData(xTC.at(0),Qt::ToolTipRole);
        xItem->setData(m_lstxItemTCase.size(),Qt::UserRole + 2); // store its initial index
        xItem->setData(xTC.at(1),Qt::UserRole + 3); // store its description
        xItem->setData(bRequired?"required":"optional",Qt::UserRole + 4); // if required TCase
        m_lstxItemTCase.push_back(xItem);
        if (bRequired)
        {

            //xItem->setData(QColor("#ff6052"),Qt::BackgroundColorRole);
            //xItem->setData(QColor(Qt::white),Qt::TextColorRole);
        }
        pxStandardModel->appendRow(xItem);
    }
    pxStandardModel->setSortRole(Qt::UserRole+2);
    pxModelForRemove->setSortRole(Qt::UserRole+2);
    pxModelForAdd->setSortRole(Qt::UserRole+2);

    //Setting the icon size
    m_pxUi->lstviewTestCases->setIconSize(QSize(20, 20));
    //Setting the model
    m_pxUi->lstviewTestCases->setModel(pxStandardModel);


    pxModelForRemove->setSourceModel(pxStandardModel);
    pxModelForRemove->setFilterRole(Qt::UserRole + 4);
    m_xRemoveTCaseDlg.m_pxUi->lstviewSelect->setModel(pxModelForRemove);
    m_xRemoveTCaseDlg.m_pxUi->lstviewSelect->setIconSize(QSize(20, 20));
    m_xRemoveTCaseDlg.m_pxUi->btnSelect->setText(tr("Remove"));
    m_xRemoveTCaseDlg.setWindowTitle(tr("Selecting the Test Cases to remove"));
    pxModelForRemove->setFilterFixedString("optional");

    m_xAddTCaseDlg.m_pxUi->lstviewSelect->setModel(pxModelForAdd);
    m_xAddTCaseDlg.m_pxUi->lstviewSelect->setIconSize(QSize(20, 20));
    m_xAddTCaseDlg.setWindowTitle(tr("Selecting the Test Cases to add"));

    m_pxUi->btnAddTC->setEnabled(false);
    m_pxUi->btnRemoveTC->setEnabled(true);
}


/**
 * Message sent by BTE to transform it into a interactive grapical component
 * Warning: obsolete replaced by onBteMessageNoModal()
**/
void CXmlGTE::onBteMessage(uint uiIdReq, QString strTitle, QString strMsg, uint eMsg, QString strVariable)
{
    qDebug() << __FUNCTION__;
    switch (eMsg)
    {
        case CBteRunner::eMSGT_CONFIRM_V1:
        {
            QMessageBox::information(this,
                                     "Testing \"" + strTitle+ "\"",
                                     strMsg,
                                     QMessageBox::Ok);
            CBteRunner::getInstance().confirmRequestResponse(uiIdReq, QVariant(true));
            break;
        }

        case CBteRunner::eMSGT_CONFIRM_V2:
        {
            int iRes = QMessageBox::question(this,
                                             "Testing \"" + strTitle+ "\"",
                                             strMsg,
                                             QMessageBox::Yes | QMessageBox::No,
                                             QMessageBox::Yes);
            CBteRunner::getInstance().confirmRequestResponse(uiIdReq, QVariant(iRes == QMessageBox::Yes) );
            break;
        }

        case CBteRunner::eMSGT_CONFIRM_V3:
        {
            int iRes = QMessageBox::question(this,
                                             strTitle,
                                             strMsg,
                                             QMessageBox::Yes | QMessageBox::No | QMessageBox::NoAll,
                                             QMessageBox::Yes);
            CBteRunner::getInstance().confirmRequestResponse(uiIdReq, QVariant( iRes == QMessageBox::Yes? 1: (iRes == QMessageBox::No ? 0:-1) ));
            break;
        }

        case CBteRunner::eMSGT_INPUT:
        {
            bool bOk;
            m_mpstrBteVar[strVariable] = CBteRunner::getInstance().getVar(strVariable);
            QString strVal = QInputDialog::getText( this,
                                                    strTitle,
                                                    strMsg,
                                                    QLineEdit::Normal,
                                                    m_mpstrBteVar[strVariable],
                                                    &bOk);
            if (bOk)
            {
                m_mpstrBteVar[strVariable] = strVal;
            }
            CBteRunner::getInstance().confirmRequestResponse(uiIdReq, QVariant(strVal));
            break;
        }

        case CBteRunner::eMSGT_INFO:
        default:
            qDebug() << "BTE_INFO\t:" << strMsg;
            break;
    }
}

/** Message sent by BTE to transform it into a interactive grapical component **/
void CXmlGTE::onBteMessageNoModal(uint uiIdReq, QString strTitle, QString strMsg, uint eMsg, QString strVariable)
{
    qDebug() << __FUNCTION__;
    m_uiBteMsgReqid = uiIdReq;
    QDialog* pxDialog = NULL;

    switch (eMsg)
    {
        case CBteRunner::eMSGT_CONFIRM_V1:
        {
            QMessageBox* pxMsgBox = new QMessageBox(QMessageBox::Information,
                                                strTitle,
                                                strMsg,
                                                QMessageBox::Ok,
                                                this,
                                                Qt::WindowStaysOnTopHint);
            connect( pxMsgBox, &QDialog::finished, this, [pxMsgBox]( int iResult) {
                    CBteRunner::getInstance().confirmRequestResponse(
                                pxMsgBox->property("ReqId").toUInt(),
                                QVariant(iResult == QMessageBox::Ok ? true : false)
                                );
                });
            pxDialog = pxMsgBox;

            break;
        }

        case CBteRunner::eMSGT_CONFIRM_V2:
        {
            QMessageBox* pxMsgBox = new QMessageBox(QMessageBox::Question,
                                                strTitle,
                                                strMsg,
                                                QMessageBox::Yes | QMessageBox::No,
                                                this);
            pxMsgBox->setDefaultButton(QMessageBox::Yes);
            connect( pxMsgBox, &QDialog::finished, this, [pxMsgBox]( int iResult) {
                    CBteRunner::getInstance().confirmRequestResponse(
                                pxMsgBox->property("ReqId").toUInt(),
                                QVariant(iResult == QMessageBox::Yes ? true : false)
                                );
                });

            pxDialog = pxMsgBox;
            break;
        }

        case CBteRunner::eMSGT_CONFIRM_V3:
        {
            QMessageBox* pxMsgBox = new QMessageBox(QMessageBox::NoIcon,
                                                strTitle,
                                                strMsg,
                                                QMessageBox::Yes | QMessageBox::No | QMessageBox::NoAll,
                                                this);
            pxMsgBox->setDefaultButton(QMessageBox::Yes);
            connect( pxMsgBox, &QDialog::finished, this, [pxMsgBox]( int iRes) {
                    CBteRunner::getInstance().confirmRequestResponse(
                                pxMsgBox->property("ReqId").toUInt(),
                                QVariant( iRes == QMessageBox::Yes? 1: (iRes == QMessageBox::No ? 0:-1) )
                                );
                });
            pxDialog = pxMsgBox;
            break;
        }

        case CBteRunner::eMSGT_INPUT:
        {
            m_mpstrBteVar[strVariable] = CBteRunner::getInstance().getVar(strVariable);

            QInputDialog* pxInpDiag = new QInputDialog();
            pxInpDiag->setWindowTitle(strTitle);
            pxInpDiag->setLabelText(strMsg);
            QStringList lstText = m_mpstrBteVar[strVariable].split("\n");
            if (lstText.size() == 1)
            {
                pxInpDiag->setTextValue(lstText.at(0));
            }
            else
            {
                pxInpDiag->setTextValue(lstText.at(0));
                pxInpDiag->setComboBoxEditable(true);
                pxInpDiag->setComboBoxItems(lstText);
            }

            connect( pxInpDiag, &QDialog::finished, this, [pxInpDiag]( int iResult) {
                    CBteRunner::getInstance().confirmRequestResponse(
                                pxInpDiag->property("ReqId").toUInt(),
                                QVariant(iResult == QDialog::Accepted ? pxInpDiag->textValue() : "")
                                );
                });
            pxDialog = pxInpDiag;
            break;
        }

        case CBteRunner::eMSGT_INFO:
        default:
            qDebug() << "BTE_INFO\t:" << strMsg;
            break;
    }

    if (pxDialog)
    {
        pxDialog->setProperty("ReqId", QVariant(uiIdReq));
        pxDialog->setAttribute(Qt::WA_DeleteOnClose); // delete pointer after close
        pxDialog->setModal(false);
        pxDialog->show();
    }
}

void CXmlGTE::on_btnAddTC_clicked()
{
    for( int i=0; i < m_lstxItemTCase.size() ;++i)
    {
        m_lstxItemTCase.at(i)->setData(QIcon(":/images/arrow16.png"),Qt::DecorationRole);
        m_lstxItemTCase.at(i)->setData(m_lstxItemTCase.at(i)->data(),Qt::ToolTipRole);
    }

    if (m_xAddTCaseDlg.exec()== QDialog::Accepted)
    {
        if (m_xAddTCaseDlg.m_pxUi->lstviewSelect->selectionModel())
        {
            QModelIndexList  xSelectedIndexes = m_xAddTCaseDlg.m_pxUi->lstviewSelect->selectionModel()->selectedIndexes();
            bool bSort = xSelectedIndexes.size();
            while(xSelectedIndexes.size())
            {
                int iRow = xSelectedIndexes[0].row(); //this will give the row of one selected Test Case.
                int iIdx = xSelectedIndexes[0].data(Qt::UserRole+2).toInt(); // get the original index
                qDebug() << "Row=" << iRow << " Index=" << iIdx;
                dynamic_cast<QStandardItemModel*>(m_xAddTCaseDlg.m_pxUi->lstviewSelect->model())->takeRow(iRow);
                dynamic_cast<QStandardItemModel*>(m_pxUi->lstviewTestCases->model())->appendRow(m_lstxItemTCase.at(iIdx));
                xSelectedIndexes = m_xAddTCaseDlg.m_pxUi->lstviewSelect->selectionModel()->selectedIndexes();
            }
            if (bSort)
            {
                dynamic_cast<QStandardItemModel*>(m_pxUi->lstviewTestCases->model())->sort(0);
                m_pxUi->btnRemoveTC->setEnabled(true);
            }
            if (m_xAddTCaseDlg.m_pxUi->lstviewSelect->model()->rowCount()==0)
            {
                m_pxUi->btnAddTC->setEnabled(false);
            }
        }
        else
        {
            m_pxUi->btnAddTC->setEnabled(false);
        }
    }
}

void CXmlGTE::on_btnRemoveTC_clicked()
{
    for( int i=0; i < m_lstxItemTCase.size() ;++i)
    {
        m_lstxItemTCase.at(i)->setData(QIcon(":/images/arrow16.png"),Qt::DecorationRole);
        m_lstxItemTCase.at(i)->setData(m_lstxItemTCase.at(i)->data(),Qt::ToolTipRole);
    }

    if (m_xRemoveTCaseDlg.exec()== QDialog::Accepted)
    {
        if (m_xRemoveTCaseDlg.m_pxUi->lstviewSelect->selectionModel())
        {
            QModelIndexList  xSelectedIndexes = m_xRemoveTCaseDlg.m_pxUi->lstviewSelect->selectionModel()->selectedIndexes();
            bool bSort = xSelectedIndexes.size();
            while(xSelectedIndexes.size())
            {
                int iRow = xSelectedIndexes[0].row(); //this will give the row of one selected Test Case.
                int iIdx = xSelectedIndexes[0].data(Qt::UserRole+2).toInt(); // get the original index
                qDebug() << "Row=" << iRow << " Index=" << iIdx;
                int iNumRows = m_pxUi->lstviewTestCases->model()->rowCount();

                //dynamic_cast<QSortFilterProxyModel*>(m_xRemoveTCaseDlg.m_pxUi->lstviewSelect->model())->removeRow(iRow);
                for (int iRowSource = 0; iRowSource < iNumRows; ++iRowSource)
                {
                    QModelIndex index = m_pxUi->lstviewTestCases->model()->index(iRowSource, 0);
                    int iDx = m_pxUi->lstviewTestCases->model()->data(index,Qt::UserRole+2).toInt();;
                    if (iIdx == iDx)
                    {
                        dynamic_cast<QStandardItemModel*>(m_pxUi->lstviewTestCases->model())->takeRow(iRowSource);
                        break;
                    }
                 }
                dynamic_cast<QStandardItemModel*>(m_xAddTCaseDlg.m_pxUi->lstviewSelect->model())->appendRow(m_lstxItemTCase.at(iIdx));
                xSelectedIndexes = m_xRemoveTCaseDlg.m_pxUi->lstviewSelect->selectionModel()->selectedIndexes();
            }
            if (bSort)
            {
                dynamic_cast<QStandardItemModel*>(m_xAddTCaseDlg.m_pxUi->lstviewSelect->model())->sort(0);
                m_pxUi->btnAddTC->setEnabled(true);
            }
            if (m_pxUi->lstviewTestCases->model()->rowCount()==0)
            {
                m_pxUi->btnRemoveTC->setEnabled(false);
            }
        }
        else
        {
            m_pxUi->btnRemoveTC->setEnabled(false);
        }

    }
}

void CXmlGTE::on_actionRepeat_toggled(bool bRepeat)
{
    CBteRunner::getInstance().setRepeatFailure(bRepeat);
}


void CXmlGTE::on_filterEditMsgText_textChanged(const QString & strArg1)
{
    qDebug() << __FUNCTION__;

    QRegExp::PatternSyntax eSyntax = m_pxUi->ckboxRegExp->isChecked()? QRegExp::RegExp : QRegExp::FixedString;
    QRegExp xRegExp(strArg1, Qt::CaseInsensitive, eSyntax);

    m_xFilterProxyModel.setFilter(5,xRegExp,Qt::DisplayRole);
    m_xFilterProxyModel.invalidate();// forces new filtering
}




void CXmlGTE::on_filterEditTCase_textChanged(const QString &strArg1)
{
    QRegExp::PatternSyntax eSyntax = m_pxUi->ckboxRegExp->isChecked()? QRegExp::RegExp : QRegExp::FixedString;
    QRegExp xRegExp(strArg1, Qt::CaseInsensitive, eSyntax);

    m_xFilterProxyModel.setFilter(4,xRegExp,Qt::DisplayRole);
    m_xFilterProxyModel.invalidate();// forces new filtering
}

void CXmlGTE::on_filterEditDevice_textChanged(const QString &strArg1)
{
    QRegExp::PatternSyntax eSyntax = m_pxUi->ckboxRegExp->isChecked()? QRegExp::RegExp : QRegExp::FixedString;
    QRegExp xRegExp(strArg1, Qt::CaseInsensitive, eSyntax);

    m_xFilterProxyModel.setFilter(3,xRegExp,Qt::DisplayRole);
    m_xFilterProxyModel.invalidate();// forces new filtering
}

void CXmlGTE::setFilterMsgType()
{
    QString xPattern;
    if (m_pxUi->btnFail->isChecked())
    {
        xPattern += "|FAIL";
    }
    if (m_pxUi->btnPass->isChecked())
    {
        xPattern += "|PASS";
    }
    if (m_pxUi->btnSkip->isChecked())
    {
        xPattern += "|SKIP";
    }
    if (m_pxUi->btnErro->isChecked())
    {
        xPattern += "|ERRO";
    }
    if (m_pxUi->btnWarn->isChecked())
    {
        xPattern += "|WARN";
    }
    if (m_pxUi->btnInfo->isChecked())
    {
        xPattern += "|INFO";
    }
    if (m_pxUi->btnMsg1->isChecked())
    {
        xPattern += "|MESG";
    }
    if (m_pxUi->btnMsg2->isChecked())
    {
        xPattern += "|TRAC";
    }
    if (!xPattern.isEmpty())
    {
        xPattern = xPattern.mid(1);
    }
    qDebug() << xPattern;
    QRegExp xRegExp(xPattern, Qt::CaseInsensitive, QRegExp::RegExp);
    m_xFilterProxyModel.setFilter(0,xRegExp,Qt::UserRole+2);
    m_xFilterProxyModel.invalidate();// forces new filtering
}

void CXmlGTE::on_btnInfo_clicked(bool)
{
    setFilterMsgType();
}

void CXmlGTE::on_btnFail_clicked(bool)
{
    setFilterMsgType();
}

void CXmlGTE::on_btnErro_clicked(bool)
{
    setFilterMsgType();
}


void CXmlGTE::on_btnPass_clicked(bool)
{
    setFilterMsgType();
}

void CXmlGTE::on_btnSkip_clicked(bool)
{
    setFilterMsgType();
}

void CXmlGTE::on_btnMsg1_clicked(bool)
{
    setFilterMsgType();
}

void CXmlGTE::on_btnMsg2_clicked(bool)
{
    setFilterMsgType();
}

void CXmlGTE::on_btnWarn_clicked(bool)
{
    setFilterMsgType();
}

void CXmlGTE::on_actionCleanAll_triggered()
{
    m_pxUi->actionStop->setEnabled(false);
    m_pxUi->actionPlay->setEnabled(false);

    m_pxUi->btnAddTC->setEnabled(false);
    m_pxUi->btnRemoveTC->setEnabled(false);

    cleanContents();
    cleanTCases();
}

void CXmlGTE::on_BtnCleanTable_clicked()
{
    cleanContents();
}


//         datetime                  Error TypeErr           ipDevice Device                TCaseId StepId            MessageText
//strLineLog = "%1" + k_strDelimiter + "%2 %3" + k_strDelimiter + "%4 %5" + k_strDelimiter + "%6 %7" + k_strDelimiter + "%8";
void CXmlGTE::on_actionSave_Log_triggered()
{
    if (m_xFilterProxyModel.rowCount())
    {
        QString strFilename = "/";
        if (m_mpstrBteVar["$SerieNum"].isEmpty())
        {
            strFilename += QString(CBteXmlParser::getInstance().getDescription()).replace(QRegExp("(\\s|[:])"),"") + "_";
        }
        else
        {
            strFilename += m_mpstrBteVar["$SerieNum"] + "_";
        }
        strFilename +=   QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + ".log";
        QString strSaveFileName = QFileDialog ::getSaveFileName(this, tr("Save Log"),QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+strFilename,tr("Log File")+ "(*.log)");

        if(!strSaveFileName.isEmpty())
        {
            QFile fOut;
            fOut.setFileName(strSaveFileName);
            fOut.open(QIODevice::WriteOnly | QIODevice::Text);

            // SOLVE format
            QTextStream xOut(&fOut);
            for(int i = 0; i<m_xFilterProxyModel.rowCount(); ++i)
            {
               int iMask = m_xFilterProxyModel.index(i,1).data(Qt::UserRole+3).toString().toInt(NULL,16);
               if ((iMask & m_strLogFileMask.toUInt(0,16)) == iMask)
               {
                   int iLevel = iMask & 0x000000ff ;
                   QString strGTError = m_xFilterProxyModel.index(i,0).data(Qt::UserRole+2).toString();

                           // date + time
                   xOut << m_xFilterProxyModel.index(i,2).data(Qt::UserRole+2).toString() << " " <<  m_xFilterProxyModel.index(i,2).data(Qt::UserRole+3).toString()
                        << CBteRunner::k_strDelimiter
                           // SolveError + SolveErrorlevel + GTEerror
                        << m_xFilterProxyModel.index(i,0).data(Qt::UserRole+3).toString() << " " << iLevel << " " << strGTError
                        << CBteRunner::k_strDelimiter
                            // ip + device
                        << m_xFilterProxyModel.index(i,3).data(Qt::UserRole+2).toString() << " " <<  m_xFilterProxyModel.index(i,3).data(Qt::UserRole+3).toString()
                        << CBteRunner::k_strDelimiter
                           // test case + step
                        << m_xFilterProxyModel.index(i,4).data(Qt::UserRole+2).toString() << ":" <<  m_xFilterProxyModel.index(i,4).data(Qt::UserRole+3).toString()
                        << CBteRunner::k_strDelimiter
                           // text
                        << "\"" + m_xFilterProxyModel.index(i,5).data(Qt::UserRole+2).toString() +"\""
                         // line in blue or not ?
                        << (strGTError=="FAIL" ? CBteRunner::k_strDelimiter+ "B" :"")
                        << endl;
               }
            }
            fOut.flush(); //empty the buffer data to the file
            fOut.close();
            QMessageBox::information(this,"BTEController",tr("Log file has been saved."),QMessageBox::Ok);
        }
        else  //select "Cancel"
        {
            return;
        }
    }
}

/** function invoked when the frame number has changed **/
void CXmlGTE::onUpdateFrameSlider()
{
    //qDebug() << __FUNCTION__;
    if (m_pxCurrentItemTCase)
    {
        m_pxCurrentItemTCase->setData(QIcon(m_xGifAnim.currentPixmap()),Qt::DecorationRole);
    }
}

void CXmlGTE::onViewModeChanged(bool bUserMode)
{
    qDebug() << __FUNCTION__ << ":" << bUserMode;
    setUserMode(bUserMode);
}


/** set the view to the user mode **/
void CXmlGTE::setUserMode(bool bActive)
{
    m_pxUi->tabViewContent->setColumnHidden(1,bActive);
    if (bActive)
    {
        m_pxUi->slidLogLevel->hide();
        m_pxUi->labDevice->hide();
        m_pxUi->labMsgtext->hide();
        m_pxUi->labTCase_Step->hide();
        m_pxUi->filterEditDevice->hide();
        m_pxUi->filterEditMsgText->hide();
        m_pxUi->filterEditTCase->hide();
        m_pxUi->ckboxRegExp->hide();
    }
    else
    {
        m_pxUi->slidLogLevel->show();
        m_pxUi->labDevice->show();
        m_pxUi->labMsgtext->show();
        m_pxUi->labTCase_Step->show();
        m_pxUi->filterEditDevice->show();
        m_pxUi->filterEditMsgText->show();
        m_pxUi->filterEditTCase->show();
        m_pxUi->ckboxRegExp->show();
    }
}

void CXmlGTE::on_slidLogLevel_valueChanged(int iLevel)
{
    static QString strToolTipPat = m_pxUi->slidLogLevel->toolTip();

    qDebug() << __FUNCTION__ << ":" << iLevel;

    // get the messages having a level lower than or equal to iLevel
    QString xPattern = QString("^[1-%1]{1}$").arg(iLevel);

    QRegExp xRegExp( (iLevel<5?xPattern:""), Qt::CaseInsensitive, QRegExp::RegExp);
    m_xFilterProxyModel.setFilter(1,xRegExp,Qt::UserRole+2);
    m_xFilterProxyModel.invalidate();// forces new filtering
    m_pxUi->slidLogLevel->setToolTip(QString(strToolTipPat).arg(iLevel));
}


/** request the device's SW versions **/
void CXmlGTE::onRequestVersion()
{
    qDebug() << __FUNCTION__;
    QAction* pxAct = dynamic_cast<QAction*>(sender());
    CBteRunner::getInstance().requestConfigInfo(pxAct->text());
    QWhatsThis::showText(pxAct->parentWidget()->pos(),pxAct->whatsThis(),pxAct->parentWidget());
}


/** notified whenever a device's detection status has changed
 * @param strDeviceName short name of the device
 * @param bIsDestected true if it's detected otherwise false.*/
void CXmlGTE::onNotifyDetectionStatus(QString strDeviceName, bool bIsDetected)
{
    qDebug() << __FUNCTION__ << " : "<<strDeviceName;
    foreach(QAction * pxAct ,m_pxUi->menuStatus->actions())
    {
        if (pxAct->text()==strDeviceName)
        {
            pxAct->setIcon(bIsDetected?QIcon(":/images/green16.png"):QIcon(":/images/red16.png"));
            if (bIsDetected && bIsDetected!=pxAct->data().toBool())
            {
                CBteRunner::getInstance().requestConfigInfo(pxAct->text());
            }
            pxAct->setData(bIsDetected);
            break;
        }
    }

}

/** response of the server about the device's versions
 * @param strDeviceName short name of the device
 * @param rgVersionName list of version names
 * @param rgVersionValue list of versions gotten */
void CXmlGTE::onNotifyDeviceVersions(QString strDeviceName, QStringList rgVersionName, QStringList rgVersionValue)
{
    qDebug() << __FUNCTION__ << " : "<<strDeviceName << " : "<< rgVersionName.join(" | ") << " : "<< rgVersionValue.join( " | ");

    foreach(QAction * pxAct ,m_pxUi->menuStatus->actions())
    {
        if (pxAct->text()==strDeviceName)
        {
            pxAct->setWhatsThis(tr("<html><body>\
                                          <h4>Versions of Device %1:</h4>\
                                            <u>\
                                                <li>SW  Version  : %2</li>\
                                                <li>OS  Version  : %3</li>\
                                                <li>RFS Version  : %4</li>\
                                            </ul>\
                                          </body></hml>").arg(strDeviceName)
                                                         .arg(rgVersionValue.at(0))
                                                         .arg(rgVersionValue.at(1))
                                                         .arg(rgVersionValue.at(2))
                              );
            break;
        }
    }
}

void CXmlGTE::on_actionAbout_XmlGTE_triggered()
{
    QDialog xAbout;
    Ui_AboutXmlGTE xAboutXmlGTE;
    xAboutXmlGTE.setupUi(&xAbout);

    xAbout.setWindowTitle("About " + CXmlGTE::k_strAppName);
    xAboutXmlGTE.lblAbout->setText(tr("<h3>%1 %2</h3>"
                              "<p>"
                              "XML-GTE is a Qt application which helps to verify the robustness of a Test system using a list of Test cases described in an XML file. <br/>"
                              "The aim of this tool is to provide an easy way to test a multiple set of remote devices which need to interact together."
                              "</p>"
                              "<p>This project is based on Qt 5.12.8, QSsh 1.0.0 and Botan 2.13.0</p>"
                              ).arg(CXmlGTE::k_strAppName)
                               .arg(CXmlGTE::k_strAppVersion));

    xAbout.exec();

    //QMessageBox::about(this,"About " + CXmlGTE::k_strAppName, strAboutAGTE);
}
