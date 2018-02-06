#ifndef DESKTOPUTIL_H
#define DESKTOPUTIL_H

#include <QString>

namespace amex {
    class desktoputil {



    public:
        desktoputil();


        /*
         * @create: true sets the desktop entry and false removes it
         * @icon: in linux provide extra file address for icon field for desktop
         *  entry
         */
        static bool set_desktop( bool create = true, const QString& iconFilePath = "");
        static bool is_desktop( void );
    };
}


#endif // DESKTOPUTIL_H
