/**********************************************************************************************
    Copyright (C) 2012 Fabrice Crohas <fcrohas@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

**********************************************************************************************/

#include <iostream>

#include <QtCore>
#include <QtGui>
#include <QRegExp>

#ifdef __MINGW32__
#undef LP
#endif

#include "CApplication.h"
#include "CMainWindow.h"

#include "CGetOpt.h"
#include "CAppOpts.h"

#include "config.h"
#include "version.h"

static void usage(std::ostream &s)
{
    s << "usage: qicompcr [-d | --debug]\n"
        "                    [-h | --help]\n"
        "                    [-m FD | --monitor=FD]\n"
        "                    [-n | --no-splash]\n"
        "                    [-c | --config=file]\n"
        "                    [files...]\n"
        "\n"
        "The monitor function will read data from files if there is input on stream FD.\n"
        "For stdin use FD=0.\n\n";
}


static void myMessageOutput(QtMsgType type, const char *msg)
{
    switch (type)
    {
        case QtDebugMsg:
            if (qlOpts->debug)
            {
                puts(msg);
            }
            break;

        case QtWarningMsg:
            fprintf(stderr, "Warning: %s\n", msg);
            break;

        case QtCriticalMsg:
            fprintf(stderr, "Critical: %s\n", msg);
            break;

        case QtFatalMsg:
            fprintf(stderr, "Fatal: %s\n", msg);
            abort();
    }
}


CAppOpts *qlOpts;

static void processOptions()
{
    CGetOpt opts;                // uses qApp->argc() and qApp->argv()
    bool dValue;
    opts.addSwitch('d', "debug", &dValue);
    bool hValue;
    opts.addSwitch('h', "help", &hValue);
    QString mValue;
    opts.addOptionalOption('m', "monitor", &mValue, "0");
    bool nValue;
    opts.addSwitch('n', "no-splash", &nValue);
    QStringList args;
    opts.addOptionalArguments("files", &args);
    QString config;
    opts.addOptionalOption('c', "config", &config, "");

    if (!opts.parse())
    {
        usage(std::cerr);
        exit(1);
    }

    if (hValue)
    {
        usage(std::cout);
        exit(0);
    }

    int m = -1;
    if (mValue != QString::null)
    {
        bool ok;
        m = mValue.toInt(&ok);
        if (!ok)
        {
            usage(std::cerr);
            exit(1);
        }
    }

    qDebug() << "use config file:" << config;
    qlOpts = new CAppOpts(dValue,// bool debug
        m,                       // int monitor
        nValue,                  // bool nosplash
        config,                  // optional config file
        args);                   // arguments
}


int main(int argc, char ** argv)
{

    QDir path(QDir::home().filePath(CONFIGDIR));
    if(!path.exists())
    {
        path.mkpath("./");
    }

    QString locale = QLocale::system().name();

#ifndef WIN32
    setenv("LC_NUMERIC","C",1);
#endif
    CApplication theApp(argc,argv);
    processOptions();

#ifndef WIN32
    qInstallMsgHandler(myMessageOutput);
#endif

#ifdef WIN32
    // setup environment variables
	QString apppath = QCoreApplication::applicationDirPath();
	apppath = apppath.replace("/", "\\");

	QString env_path = qgetenv("PATH");
    env_path += QString(";%1;").arg(apppath);
	qputenv("PATH", env_path.toUtf8());

#endif


#ifdef ENABLE_TRANSLATION
    {
        QString resourceDir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
        QTranslator *qtTranslator = new QTranslator(0);
        if (qtTranslator->load(QLatin1String("qt_") + locale,resourceDir))
        {
            theApp.installTranslator(qtTranslator);
        }
        else if (qtTranslator->load(QLatin1String("qt_") + locale,QCoreApplication::applicationDirPath()))
        {
            theApp.installTranslator(qtTranslator);
        }

        QStringList dirList;
#ifndef Q_WS_MAC
        dirList << ".";
        dirList << "src";
#ifndef Q_OS_WIN32
        dirList << QCoreApplication::applicationDirPath().replace(QRegExp("bin$"), "share/qicompcr/translations");
#else
        dirList << QCoreApplication::applicationDirPath();
#endif
#else
        dirList << QCoreApplication::applicationDirPath().replace(QRegExp("MacOS$"), "Resources");
#endif
        QTranslator *qlandkartegtTranslator = new QTranslator(0);
        qDebug() << dirList;
        foreach(QString dir, dirList)
        {
            QString transName = QLatin1String("qicompcr_") + locale;
            if (qlandkartegtTranslator->load( transName, dir))
            {
                theApp.installTranslator(qlandkartegtTranslator);
                qDebug() << "using file '"+ QDir(dir).canonicalPath() + "/" + transName + ".qm' for translations.";
                break;
            }
        }
    }
#endif

    QCoreApplication::setApplicationName("QIComPCR");
    QCoreApplication::setOrganizationName("QIComPCR");
    QCoreApplication::setOrganizationDomain("qicompcr.org");
    QApplication::setWindowIcon(QIcon(":/icons/qicompcr.png"));

#ifdef WIN32
    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath());
    qDebug() << QCoreApplication::applicationDirPath();
    qDebug() << QCoreApplication::libraryPaths();
#endif
#ifdef WITH_GUI
    QSplashScreen *splash = 0;
    if (!qlOpts->nosplash)
    {
        QPixmap pic(":/pics/splash.png");
        QPainter p(&pic);
        QFont f = p.font();
        f.setBold(true);

        p.setPen(Qt::white);
        p.setFont(f);
        p.drawText(400,370,"V " VER_STR);

        splash = new QSplashScreen(pic);
        splash->show();
    }
    CMainWindow w;
    w.show();
    if (splash != 0)
    {
        splash->finish(&w);
        delete splash;
    }
#endif
    int res  = theApp.exec();

    //delete qlOpts;

    return res;
}

