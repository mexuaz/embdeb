#include "srvutil.h"

#include "fileentry.h"

#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QDir>
#include <QStandardPaths>
#include <QProcess>

#include <unistd.h> // link()

using namespace amex;

srvutil::srvutil()
{

}

bool srvutil::set_service(bool create /*= true*/, const char *exec /*= ""*/, const char *comment /*= ""*/) {
#if defined(Q_OS_LINUX)
    QString srvEntryFile("/lib/systemd/" + QCoreApplication::applicationName() + ".service");
    QString srvLinkFile("/etc/systemd/system/" + QCoreApplication::applicationName() + ".service");
    if(create) {

        // create the relating files to that service
        if(!QFile::exists(srvEntryFile) && !amex::fileentry::create_srvunit_entry(
                    srvEntryFile.toUtf8().constData(),
                    strlen(exec) > 0 ? exec : QCoreApplication::applicationFilePath().toUtf8().constData(),
                    strlen(comment) > 0 ? comment : QCoreApplication::applicationName().toUtf8().constData())) {
            qCritical() << "The application could not create unit (service) entry file.";
            return false;
        }

        // Now we should create hard link to the original service file

        if(::link(srvEntryFile.toUtf8().constData(), srvLinkFile.toUtf8().constData()) < 0) {
            qCritical() << "The application could not hard link service entry file in system directory.";
            qCritical() << strerror(errno);
            return false;
        }

        // reload services list
        QProcess pr;
        pr.start("systemctl" , QStringList("daemon-reload"));
        pr.waitForFinished(-1);

        // start and enable current service
        QStringList ls;
        ls << "start" << QCoreApplication::applicationName() + ".service";
        pr.start("systemctl" , ls);
        pr.waitForFinished(-1);

        ls[0] = "enable";
        pr.start("systemctl" , ls);
        pr.waitForFinished(-1);

        return true;
    } else {

        // stop the service
        QProcess pr;
        QStringList ls;
        ls << "stop" << QCoreApplication::applicationName() + ".service";
        pr.start("systemctl" , ls);
        pr.waitForFinished(-1);

        // remove the relating files to that service
        if(!QFile(srvLinkFile).remove()) {
            qCritical() << "The application could not delete the link to service entry file: " << srvLinkFile;
            return false;
        }

        if(!QFile(srvEntryFile).remove()) {
            qCritical() << "The application could not delete the unit (service) entry file: " << srvEntryFile;
            return false;
        }

        // reload the services list
        pr.start("systemctl" , QStringList("daemon-reload"));
        pr.waitForFinished(-1);

        return true;
    }
#elif defined(Q_OS_WIN)
    qCritical() << __FUNCTION__ << "(" << create << ", " << exec << ", " << comment << ") => Unsupported OS";
    return false;
#endif
}

bool srvutil::is_service() {

#if defined(Q_OS_LINUX)
    QString srvEntryFile("/lib/systemd/" + QCoreApplication::applicationName() + ".service");
    QString srvLinkFile("/etc/systemd/system/" + QCoreApplication::applicationName() + ".service");
    if(QFile::exists(srvEntryFile) && QFile::exists(srvLinkFile)) {
        return true;
    }
    return false;
#elif defined(Q_OS_WIN)
    qCritical() << __FUNCTION__ << "Unsupported OS";
    return false;
#endif
}
