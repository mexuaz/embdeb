#include "sudocommander.h"

#include <QProcess>
//#include <QInputDialog>
#include <QDebug>
#include <QSettings>

#include "simplecrypt.h"

#define PASSWORDPROFILEKEY "RP542"

bool sudoCommander::run(const QString &cmd, const QString &sudopass, QByteArray *std_out, QByteArray *std_err)
{
    QProcess process_echo_pass;
    QProcess process_sudo_cmd;

    process_echo_pass.setStandardOutputProcess(&process_sudo_cmd);


    process_sudo_cmd.start("sudo -S " + cmd);
    process_echo_pass.start("echo " + sudopass);


    if(!process_echo_pass.waitForFinished(-1)) {
        qCritical() << process_echo_pass.errorString();
        return false;
    }

    if(!process_sudo_cmd.waitForFinished(-1)) {
        qCritical() << process_sudo_cmd.errorString();
        return false;
    }

    if(std_out) {
        std_out->clear();
        std_out->append(process_sudo_cmd.readAllStandardOutput());
    }

    if(std_err) {
        std_err->clear();
        std_err->append(process_sudo_cmd.readAllStandardError());
    }

    return true;
}

bool sudoCommander::is_sudo_password(const QString &pass)
{
    QByteArray buffer;
    if(!run("echo Hello", pass, &buffer)) {
        return false;
    }

    if(!buffer.startsWith("Hello")) {
        return false;
    }

    return true;
}

void sudoCommander::save_to_profile_pass(const QString &pass)
{
    SimpleCrypt crypto(Q_UINT64_C(0x0cd257ab954af47)); //some random number

    QSettings settings;
    settings.setValue(PASSWORDPROFILEKEY, QVariant(crypto.encryptToString(pass)));
}


bool sudoCommander::run_with_profile_pass(const QString& cmd, QByteArray* cmd_output /*= Q_NULLPTR*/)
{
    SimpleCrypt crypto(Q_UINT64_C(0x0cd257ab954af47)); //some random number

    QSettings settings;
    QString sudoPass = settings.value(PASSWORDPROFILEKEY, QVariant("")).toString();
    sudoPass = crypto.decryptToString(sudoPass);
    if(!is_sudo_password(sudoPass)) {
        qCritical() << "Sudo password was worng!";
        return false;
    }

    return run(cmd, sudoPass, cmd_output);
}




