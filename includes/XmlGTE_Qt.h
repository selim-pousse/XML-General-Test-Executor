//=================================================================================================
// Copyright (C) 2016 Pousse Systems
//
// Name           CSelectTestcaseDlg
//
// Description    Main Window of the tool.
//
// Author         Selim Pousse
//=================================================================================================
#ifndef XmlGTE_H
#define XmlGTE_H


#include <QMainWindow>
#include <QFile>
#include <QDebug>
#include <QLabel>
#include <Logger.h>
#include <QMovie>
#include <SelectTestcaseDlg.h>
#include <BteViewSortFilterProxyModel.h>
#include <QStandardItem>
#include <QSortFilterProxyModel>
#include <QMessageBox>


namespace Ui
{
    class CXmlGTE;
}

class CXmlGTE : public QMainWindow
{
    Q_OBJECT

private:
    /** pointer for graphical components **/
    Ui::CXmlGTE* m_pxUi;

    /** xml file loaded by the user **/
    QString m_strCurrentFile;

    /** store the inputs requested to the user **/
    QMap<QString,QString> m_mpstrBteVar;

    /** Id of the waiting request to the user */
    uint m_uiBteMsgReqid;

    /** gif animated for loading **/
    QMovie m_xGifAnim;

    /** list of the Test Cases defined */
    QList<QStandardItem*> m_lstxItemTCase;

    /** for adding test cases **/
    CSelectTestcaseDlg m_xAddTCaseDlg;

    /** for removing test cases **/
    CSelectTestcaseDlg m_xRemoveTCaseDlg;

    /** model related to the component TabViewContent **/
    QStandardItemModel m_xTabViewModel;

    /** For filtering contents **/
    CBteViewSortFilterProxyModel m_xFilterProxyModel;

    /** show the label of the compagny **/
    QLabel m_xAnnaxWidg;

    /** customized spacer destined for the toolbar  **/
    QWidget m_xSpacerWidg;

    /** The logger */
    CLogger m_xLogger;

    /**  mask of the logger */
    QString m_strLogFileMask;

    /** Current running Test Case */
    QStandardItem * m_pxCurrentItemTCase;

    // static variables
    /** Application name **/
    static const QString k_strAppName;

    /** log mask **/
    static const QString k_strKeyLogMask;

    /** Application version **/
    const QString k_strAppVersion;

public:
    /** Constructor **/
    explicit CXmlGTE(QString strAppVersion, QWidget *parent = 0);

    /** Destructor **/
    ~CXmlGTE();

    /** function getter of m_xCurrentFile */
    inline const QString& getCurrentFile() { return m_strCurrentFile;}

    /** initialize the programm as a plugin for the given device **/
    void initPluginForDevice(const QString& strXmlFile);

    /** initialize the view mode as user (restricted)  **/
    void initUserMode();

    /** set the mask of the logger */
    void setLogFileMask(const QString& strMask) {m_strLogFileMask = strMask;}

    /** restore the log mask **/
    void restoreMask();

private:
    void readyToStartTestProcedure();

    void startTestProcedure();

    /** initialization of the components for selecting, adding and removing the testcases **/
    void initTestCases();

    /** clean the models and contents of the TableView component **/
    void cleanContents();

    /** clean the models and data related to the Testcases **/
    void cleanTCases();

    /** return the associated Item of the given TestCase ID **/
    QStandardItem* getItemTCase(const QString& strIdTestCase, int &iIndex);

    /** set the view to the user mode **/
    void setUserMode(bool bActive);

private slots:

    /** add new row into the tableView
     * @param qstrList list of values for each column
     */
    void appendTabView(const QStringList & qstrList);

    /**
     * Message sent by BTE to transform it into a interactive grapical component
     * Warning: obsolete replaced by onBteMessageNoModal()
    **/
    void onBteMessage(uint uiIdReq, QString strTitle, QString strMsg, uint eMsg, QString strVariable = QString());

    /** Message sent by BTE to transform it into a interactive grapical component **/
    void onBteMessageNoModal(uint uiIdReq, QString strTitle, QString strMsg, uint eMsg, QString strVariable = QString());

    /** the test Procedure requested is done **/
    void onTestProcedureDone(bool bSuccess);

    /** inform whenever a Testcase is started */
    void onTestCaseStarted(QString strTestCaseId, QString strTxtTestCase);

    /** inform whenever a Testcase is done */
    void onTestCaseDone(QString strTestCaseId, QString strTxtTestCase, bool bSuccess, QString strReason);

    /** function for tracing and logging */
    void onAppendLogText(QString strMsg, uint eDebugLev);

    /** function invoked when the frame number has changed **/
    void onUpdateFrameSlider();

    void on_actionOpen_triggered();

    void on_actionPlay_triggered();

    void on_actionStop_triggered();

    void on_actionExit_triggered();

    void on_btnAddTC_clicked();

    void on_btnRemoveTC_clicked();

    void on_actionRepeat_toggled(bool bRepeat);

    void on_filterEditMsgText_textChanged(const QString &strArg1);
    void on_filterEditTCase_textChanged(const QString &strArg1);
    void on_filterEditDevice_textChanged(const QString &strArg1);
    void setFilterMsgType();
    void on_btnInfo_clicked(bool);
    void on_btnFail_clicked(bool);
    void on_btnErro_clicked(bool);
    void on_btnPass_clicked(bool);
    void on_btnSkip_clicked(bool);
    void on_btnMsg1_clicked(bool);
    void on_btnMsg2_clicked(bool);
    void on_btnWarn_clicked(bool);
    void on_actionCleanAll_triggered();
    void on_BtnCleanTable_clicked();
    void on_actionSave_Log_triggered();
    void onViewModeChanged(bool bUserMode);
    void on_slidLogLevel_valueChanged(int iLevel);

    /** request the device's SW versions **/
    void onRequestVersion();

    /** notified whenever a device's detection status has changed
     * @param strDeviceName short name of the device
     * @param bIsDestected true if it's detected otherwise false.*/
    void onNotifyDetectionStatus(QString strDeviceName, bool bIsDetected);

    /** response of the server about the device's versions
     * @param strDeviceName short name of the device
     * @param rgVersionName list of version names
     * @param rgVersionValue list of versions gotten */
    void onNotifyDeviceVersions(QString strDeviceName, QStringList rgVersionName, QStringList rgVersionValue);

    /** when a new device's informations has been collected
     **  it reinitialize the components aimed to detect the devices' status **/
    void onCollectionOfDevice(QList<QStringList> xDeviceContainer);
    void on_actionAbout_XmlGTE_triggered();
};




#endif // XmlGTE_H
