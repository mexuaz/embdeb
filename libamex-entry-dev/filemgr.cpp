#include "filemgr.h"


#include <QtGlobal>


using namespace amex;


const char *filemgr::file_manager()
{
#if defined(Q_OS_WIN32)
        return "explorer";
#elif defined(Q_OS_LINUX)
        return "xdg-open";
#elif defined(Q_OS_MAC)
        return "open";
#endif
}


