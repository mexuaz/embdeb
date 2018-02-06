/*
 * dskentry.h
 *
 *  Created on: Aug 14, 2015
 *      Author: Amir Mehrafsa
 */


#ifndef FILEENTRY_H
#define FILEENTRY_H

#include <QtGlobal>

namespace amex {
    class fileentry {

    public:
        fileentry( void );
        ~fileentry( void );
#if !defined(Q_OS_WIN32)
        /*
         * Upon successful completion, true shall be returned;
         * otherwise, false shall be returned and errno set to indicate the error.
         */
        static bool create_desktop_entry(const char* filename,
                                                     const char* name,
                                                     const char* exec,
                                                     const char* genericName = "",
                                                     const char* comment = "",
                                                     const char* icon = "",
                                                     const char* categories = "",
                                                     const char* extras = ""
                                                     );


        static bool create_srvunit_entry(const char* filename,
                                         const char* exec,
                                         const char* description,
                                         const char* extras = "");
#endif
    }; // end of class
} // end of namespace




#endif // FILEENTRY_H
