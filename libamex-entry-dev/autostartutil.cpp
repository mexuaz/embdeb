#include "autostartutil.h"
#include "fileentry.h"

#include <QtGlobal>
#include <QSettings>
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QStandardPaths>




#if defined(Q_OS_WIN)
#include <shlobj.h>
#endif


#define _STARTUP_ROOT_KEY_SYSTEM "HKEY_LOCAL_MACHINE"
#define _STARTUP_ROOT_KEY_USER "HKEY_CURRENT_USER"

#define _STARTUP_PATH_KEY "\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"
#define _STARTUP_PATH_KEY_X64 "\\Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Run"
#define _LOGIN_PATH_KEY "\\Software\\Microsoft\\WindowsNT\\CurrentVersion\\WinLogon"

#define _STARTUP_PATH_USER_AFTERHOME ".config/autostart"
#define _STARTUP_PATH_EXTENTION ".desktop"
#define _STARTUP_PATH_SYSTEM "/etc/xdg/autostart"


/*
 * windows user startup path C:\Users\<username>\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\
 * windows all users startup path C:\ProgramData\Microsoft\Windows\Start Menu\Programs\StartUp
 * Refrences:
 * http://stackoverflow.com/questions/33089959/get-path-to-all-users-start-menu-on-windows
 * http://doc.qt.io/qt-5/qstandardpaths.html
 */


using namespace amex;

// Runtime registry path dependening on MS Windows os architecture
QString autostart_registry_user()
{
    QString regPath(_STARTUP_ROOT_KEY_USER);
#if defined(Q_OS_WIN)
    if(QSysInfo::currentCpuArchitecture() == "x86_64") {
        regPath.append(_STARTUP_PATH_KEY_X64);
    } else { // if(QSysInfo::currentCpuArchitecture() == "i386")
        regPath.append(_STARTUP_PATH_KEY);
    }
#endif
    return regPath;
}

// Runtime registry path dependening on MS Windows os architecture
QString autostart_registry_system()
{
    QString regPath(_STARTUP_ROOT_KEY_SYSTEM);
#if defined(Q_OS_WIN)
    if(QSysInfo::currentCpuArchitecture() == "x86_64") {
        regPath.append(_STARTUP_PATH_KEY_X64);
    } else { // if(QSysInfo::currentCpuArchitecture() == "i386")
        regPath.append(_STARTUP_PATH_KEY);
    }
#endif
    return regPath;
}


bool autostartutil::set_autostart(autostart_t type, bool create /*= true*/)
{
    switch(type) {
    case autostart_t::linux_startupfolder_user:
    case autostart_t::linux_startupfolder_system:
    {
        QString autostartDir(_STARTUP_PATH_SYSTEM);
        if(type==autostart_t::linux_startupfolder_user) {
            autostartDir = QDir::homePath() + QDir::separator() + _STARTUP_PATH_USER_AFTERHOME;
        }

        QString autostartFile(autostartDir + QDir::separator() + QCoreApplication::applicationName() + _STARTUP_PATH_EXTENTION);
        if(create) {

            if(!QDir(autostartDir).exists() && !QDir().mkpath(autostartDir)) {
                qCritical() << "The application could not create to the autostart directory: " << autostartDir;
                return false;
            }

#if !defined(Q_OS_WIN32)
            if(!amex::fileentry::create_desktop_entry(
                        autostartFile.toUtf8().constData(),
                        QCoreApplication::applicationName().toUtf8().constData(),
                        QCoreApplication::applicationFilePath().toUtf8().constData(),
                        QCoreApplication::applicationName().toUtf8().constData())) {
                qCritical() << "The application could not create desktop entry file.";
                return false;
            }
#endif
            return true;

        } else {

            if(!QDir(autostartDir).exists()) {
                return true;
            }

            if(!QFile(autostartFile).remove()) {
                qCritical() << "The application could not delete the autostart file: " << autostartFile;
                return false;
            }
            return true;
        }

    }
    case autostart_t::windows_registry_user:
    case autostart_t::windows_registry_system:
    {
        QSettings settings((type == autostart_t::windows_registry_user) ? autostart_registry_user() : autostart_registry_system(), QSettings::NativeFormat);

        QString regval("\""+QCoreApplication::applicationFilePath()+"\"");
        regval.replace("/","\\");
        //regval = QDir::toNativeSeparators(regval);

        create ? settings.setValue(QCoreApplication::applicationName(), regval) : settings.remove(QCoreApplication::applicationName());

        settings.sync();
        return true;
    }
    case autostart_t::windwos_startupfolder_user:
    case autostart_t::windows_startupfolder_allusers:
    {
        QFileInfo fileInfo(QCoreApplication::applicationFilePath());
        QString ApplicationLnkPath;
        if(type == autostart_t::windwos_startupfolder_user) {
            QStringList pls = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation);
            if(!pls.size()) {
                 qCritical() << "The application could not retrieve the user application entry directory.";
                return false;
            }
            ApplicationLnkPath = pls[0];
        } else if(type == autostart_t::windows_startupfolder_allusers) {
#if defined(Q_OS_WIN)
        TCHAR startMenuPath[MAX_PATH];
        HRESULT result = SHGetFolderPath(NULL, CSIDL_COMMON_PROGRAMS, NULL, 0, startMenuPath);
         if (!SUCCEEDED(result)) {
             qCritical() << "The application could not retrieve all users application entry directory.";
             return false;
         }
         ApplicationLnkPath = QString::fromWCharArray(startMenuPath);
#endif
        }
        if(ApplicationLnkPath.isEmpty()) {
            qCritical() << "The application could not retrieve application entry directory.";
            return false;
        }

        QString linkLoc(ApplicationLnkPath + QDir::separator() + "Startup" + QDir::separator() + fileInfo.completeBaseName() + ".lnk");
        if(create) {
            if(!QFile::link(QCoreApplication::applicationFilePath(), linkLoc)) {
                qCritical() << "The application could not create startup shortcut.";
                return false;
            }
            return true;
        } else {
            if(!QFile::exists(linkLoc)) {
                return true;
            }
            if(!QFile::remove(linkLoc)) {
                qCritical() << "The application could not remove startup shortcut.";
                return false;
            }
            return true;
        }
    }
    default:
        qCritical() << "The startup type is not supported by the library.";
        return false;
    }
}

bool autostartutil::is_autostart(autostart_t type)
{
    switch(type) {
    case autostart_t::linux_startupfolder_user:
    case autostart_t::linux_startupfolder_system:
    {
        QString autostartDir(_STARTUP_PATH_SYSTEM);
        if(type==autostart_t::linux_startupfolder_user) {
            autostartDir = QDir::homePath() + QDir::separator() + _STARTUP_PATH_USER_AFTERHOME;
        }
        QFile autostartFile(autostartDir + QDir::separator() + QCoreApplication::applicationName() + _STARTUP_PATH_EXTENTION);

        if(QDir(autostartDir).exists() && autostartFile.exists()) {
            return true;
        }
        return false;
    }
    case autostart_t::windows_registry_system:
    case autostart_t::windows_registry_user:
    {
        QSettings settings((type == autostart_t::windows_registry_user) ? autostart_registry_user() : autostart_registry_system(), QSettings::NativeFormat);

        QString regval("\""+QCoreApplication::applicationFilePath()+"\"");
        regval.replace("/","\\");
        //regval = QDir::toNativeSeparators(regval);

        if( settings.value(QCoreApplication::applicationName()).isValid() && settings.value(QCoreApplication::applicationName()).toString() == regval) {
            return true;
        }

        return false;
    }
    case autostart_t::windwos_startupfolder_user:
    case autostart_t::windows_startupfolder_allusers:
    {
        QFileInfo fileInfo(QCoreApplication::applicationFilePath());
        QString ApplicationLnkPath;
        if(type == autostart_t::windwos_startupfolder_user) {
            QStringList pls = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation);
            if(!pls.size()) {
                 qCritical() << "The application could not retrieve the user application entry directory.";
                return false;
            }
            ApplicationLnkPath = pls[0];
        } else if(type == autostart_t::windows_startupfolder_allusers) {
#if defined(Q_OS_WIN)
        TCHAR startMenuPath[MAX_PATH];
        HRESULT result = SHGetFolderPath(NULL, CSIDL_COMMON_PROGRAMS, NULL, 0, startMenuPath);
         if (!SUCCEEDED(result)) {
             qCritical() << "The application could not retrieve all users application entry directory.";
             return false;
         }
         ApplicationLnkPath = QString::fromWCharArray(startMenuPath);
#endif
        }
        if(ApplicationLnkPath.isEmpty()) {
            qCritical() << "The application could not retrieve application entry directory.";
            return false;
        }
        QString linkLoc(ApplicationLnkPath + QDir::separator() + "Startup" + QDir::separator() + fileInfo.completeBaseName() + ".lnk");
        if(QFile::exists(linkLoc)) {
            return true;
        }
        return false;
    }
    default:
        return false;
    }
}



bool autostartutil::set_autostartfolder_user(bool create /*= true*/)
{
#if defined(Q_OS_WIN)
    return set_autostart(autostart_t::windwos_startupfolder_user, create);
#elif defined(Q_OS_LINUX)
    return set_autostart(autostart_t::linux_startupfolder_user, create);
#endif
}

bool autostartutil::is_autostartfolder_user( void )
{
#if defined(Q_OS_WIN)
    return is_autostart(autostart_t::windwos_startupfolder_user);
#elif defined(Q_OS_LINUX)
    return is_autostart(autostart_t::linux_startupfolder_user);
#endif
}
