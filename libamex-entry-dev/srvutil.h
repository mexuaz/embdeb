#ifndef SRVUTIL_H
#define SRVUTIL_H

namespace amex {
    class srvutil {
    public:
        srvutil();


        /*
         * @create: true sets the service and false removes it
         */
        static bool set_service( bool create = true, const char* exec = "", const char* comment = "");
        static bool is_service( void );
    };
}


#endif // SRVUTIL_H
