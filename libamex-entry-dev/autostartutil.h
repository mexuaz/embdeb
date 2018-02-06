

#ifndef _AUTOSTARTUTIL_H
#define _AUTOSTARTUTIL_H


/*
 * Statup utility package
 * written by: Amir Mehrafsa (mehrafsa@live.com)
 * version 0.1
 */

namespace amex {
    class autostartutil {

    public:

        enum class autostart_t {
            linux_startupfolder_user = 11, /*user login x11*/
            linux_startupfolder_system = 12, /*any user login x11*/

            windows_registry_user = 21, /*user login*/
            windows_registry_system = 22, /*any user login*/
            windwos_startupfolder_user = 23, /*user login*/
            windows_startupfolder_allusers = 24 /*any user login*/

        };

        /*
         * @type: is one of the enumerations of autostart_t
         * autostart_linux_startupfolder_system, autostart_windows_registry_system and autostart_windows_startupfolder_allusers options need root or adminitrators privillage in linux or windows OS
         * For windows OS the application should pass UAC to set autostart_windows_registry_system or autostart_windows_startupfolder_allusers
         * And for linux you should be root or run the application with sudo in order to use autostart_linux_startupfolder_system option
         * @create: true to set auto start option, false to remove autostart option
         */
        static bool set_autostart(autostart_t type, bool create = true);

        /*
         * @type: is one of the enumerations of autostart_t
         * This method does not need any root or administrator privillages
         */
        static bool is_autostart(autostart_t type);


        /*
         * These methods are corssplatform wrappers for set_autostart and is_autostart with startfolder and user presetted option and do not need any root or administrator privaillages
         */
        static bool set_autostartfolder_user(bool create = true);
        static bool is_autostartfolder_user( void );


    }; // end of class
}





#endif // _AUTOSTARTUTIL_H

