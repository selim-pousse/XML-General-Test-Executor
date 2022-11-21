#ifndef AGTE_H
#define AGTE_H

#include <QObject>
#include <Logger.h>
#include <QTextStream>

class CAgte : public QObject
{
    Q_OBJECT

private:

    // static variables
    /** Application name **/
    static const QString k_strAppName;

    /** the Logger */
    CLogger m_xLogger;

    /** redirect the log content to the console **/
    bool m_bShowLog;

    /** stream for the stdin **/
    QTextStream m_xCin;

    /** stream for the stdout **/
    QTextStream m_xCout;

private:
     void initialize();

public:
    explicit CAgte(QObject *parent = 0);

signals:

private slots:
     /** Message sent by BTE to transform it into a interactive grapical component **/
     void onBteMessage(uint uiIdReq, QString strTitle, QString strMsg, uint eMsg, QString strVariable = QString());

     /** the test Procedure requested is done **/
     void onTestProcedureDone(bool bSuccess);

     /** function for tracing and logging */
     void onAppendLogText(QString strMsg, uint uiMask);
};

#endif // AGTE_H
