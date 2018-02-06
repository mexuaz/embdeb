#include "desktoputil.h"

#include "fileentry.h"

#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QDir>
#include <QStandardPaths>

using namespace amex;

desktoputil::desktoputil()
{

}

bool desktoputil::set_desktop(bool create /*= true*/, const QString& iconFileName /*= ""*/) {

    QString desktopDir(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation)[0]);
    QString desktopEntryFile(desktopDir + QDir::separator() + QCoreApplication::applicationName());

#ifdef Q_OS_LINUX
    desktopEntryFile.append(".desktop");
#endif

#ifdef Q_OS_WIN32
    desktopEntryFile.append(".lnk");
#endif

    if(create) {

        if(!QDir(desktopDir).exists()) {
            qCritical() << "Desktop Directory not found: " << desktopDir;
            return false;
        }

#ifdef Q_OS_LINUX
        if(!QFile::exists(desktopEntryFile) && !amex::fileentry::create_desktop_entry(
                    desktopEntryFile.toUtf8().constData(),
                    QCoreApplication::applicationName().toUtf8().constData(),
                    QCoreApplication::applicationFilePath().toUtf8().constData(),
                    QCoreApplication::applicationName().toUtf8().constData(),
                    "",
                    iconFileName.toUtf8().constData())) {
            qCritical() << "The application could not create desktop entry file.";
            return false;
        }
#endif



#ifdef Q_OS_WIN32
        if(!QFile::link(QCoreApplication::applicationFilePath().toUtf8().constData(), desktopEntryFile)) {
            qCritical() << "The application could not create desktop entry file.";
            return false;
        }

        if(strlen(iconFileName.toUtf8().constData()) > 0) {
            qWarning() << "In MS Windows OS we do not need icon file.";
        }

#endif
        return true;

    } else {

        if(!QFile::exists(desktopEntryFile)) {
            return true;
        }

        if(!QFile(desktopEntryFile).remove()) {
            qCritical() << "The application could not delete the desktop entry file: " << desktopEntryFile;
            return false;
        }
        return true;
    }
}

bool desktoputil::is_desktop()
{
    QString desktopDir(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation)[0]);
    QString desktopEntryFile(desktopDir + QDir::separator() + QCoreApplication::applicationName());

#ifdef Q_OS_LINUX
    desktopEntryFile.append(".desktop");
#endif

#ifdef Q_OS_WIN32
    desktopEntryFile.append(".lnk");
#endif

    if(QDir(desktopDir).exists() && QFile::exists(desktopEntryFile)) {
        return true;
    }
    return false;
}
