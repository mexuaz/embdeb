#ifndef QSUDOCOMMANDER_H
#define QSUDOCOMMANDER_H

#include <QString>


namespace sudoCommander {

    /*
     * Run a sudo command with the given sudo password
     */
    bool run( const QString& cmd, const QString& sudopass
              , QByteArray* std_out = Q_NULLPTR
              , QByteArray* std_err = Q_NULLPTR );


    /*
     * Checks if the given pass in the argument is the sudo password
     */
    bool is_sudo_password( const QString& pass );



    /*
     * save the pass to the user's profile
     */
    void save_to_profile_pass(const QString& pass);

    /*
     * Cheks the application porfile for stored sudo password
     * Execute the cmd with stored sudo password
     */
    bool run_with_profile_pass(const QString& cmd, QByteArray* cmd_output = Q_NULLPTR);
}

#endif // QSUDOCOMMANDER_H
