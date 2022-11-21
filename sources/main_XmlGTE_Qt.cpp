
//=================================================================================================
// Copyright (C) 2016 Pousse Systems
//
// Name           Main.cpp
//
// Description    main function called at program startup
//
// Author         Selim Pousse
//=================================================================================================
#include "XmlGTE_Qt.h"
#include <QApplication>
#include <QtMessageHandler>
#include <QDateTime>
#include "BteXmlParser.h"
#include <QDir>
#include <QSettings>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qDebug() << argc << *argv;

    QString strFileName = CBteXmlParser::getInstance().directoryOf("configfiles").path() + "/sysConfig.ini";
    QSettings xSetting(strFileName,QSettings::IniFormat);
    QString strUseGTE       = xSetting.value("XmlGTE.use","standAlone").toString();
    QString strMode         = xSetting.value("XmlGTE.viewMode","expert").toString();
    QString strAppVersion   = xSetting.value("XmlGTE.version","Unknown").toString();
    QString strMask         = xSetting.value("XmlGTE.logFile.mask","0xfffffbff").toString();

    CXmlGTE xGTE(strAppVersion);

    xGTE.setLogFileMask(strMask);
    if (argc >= 2 && strUseGTE =="plugin")
    {
        xGTE.initPluginForDevice(argv[1]);
    }
    if (strMode =="user")
    {
        xGTE.initUserMode();
    }
    xGTE.show();

    return app.exec();
}

