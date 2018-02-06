#include "fileentry.h"


#include <fcntl.h>  // open() O_RDONLY ...
#include <unistd.h> // lseek() read() write() close()
#include <string.h> // strlen()
#include <sys/stat.h> //fchmod()



using namespace amex;


fileentry::fileentry()
{

}

fileentry::~fileentry()
{

}

#if !defined(Q_OS_WIN32)

bool fileentry::create_desktop_entry(
        const char *filename,
        const char *name,
        const char *exec,
        const char *genericName /*= ""*/,
        const char *comment /*= ""*/,
        const char *icon /*= ""*/,
        const char *categories /*= ""*/,
        const char *extras /*= ""*/)
{

    int fd = ::open(filename, O_WRONLY | O_CREAT, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    if(fd < 0) {
        return false;
    }

    if( ::fchmod(fd, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) < 0) {
        return false;
    }

    const char* l1 = {"[Desktop Entry]\r\nEncoding=UTF-8\r\nVersion=1.0\r\nName="};
    if (::write(fd, l1, strlen(l1)) < 0) {
        return false;
    }

    if (::write(fd, name, strlen(name)) < 0) {
        return false;
    }

    const char* l2 = {"\r\nGenericName="};
    if (::write(fd, l2, strlen(l2)) < 0) {
        return false;
    }

    if (::write(fd, genericName, strlen(genericName)) < 0) {
        return false;
    }

    const char* l3 = {"\r\nComment="};
    if (::write(fd, l3, strlen(l3)) < 0) {
        return false;
    }

    if (::write(fd, comment, strlen(comment)) < 0) {
        return false;
    }

    const char* l4 = {"\r\nExec="};
    if (::write(fd, l4, strlen(l4)) < 0) {
        return false;
    }

    if (::write(fd, exec, strlen(exec)) < 0) {
        return false;
    }

    const char* l5 = {"\r\nIcon="};
    if (::write(fd, l5, strlen(l5)) < 0) {
        return false;
    }

    if (::write(fd, icon, strlen(icon)) < 0) {
        return false;
    }


    const char* l6 = {"\r\nCategories="};
    if (::write(fd, l6, strlen(l6)) < 0) {
        return false;
    }

    if (::write(fd, categories, strlen(categories)) < 0) {
        return false;
    }

    //X-GNOME-Autostart-enabled=true
    const char* l9 = {"\r\nStartupNotify=false\r\nTerminal=false\r\nHidden=false\r\nType=Application\r\n"};
    if (::write(fd, l9, strlen(l9)) < 0) {
        return false;
    }

    if(strlen(extras) > 0) {
        if (::write(fd, extras, strlen(extras)) < 0) {
            return false;
        }
    }

    if (::close(fd) < 0) {
        return false;
    }

    return true;
}

bool fileentry::create_srvunit_entry(const char *filename, const char *exec,
                                     const char *description, const char *extras /*= ""*/)
{

    int fd = ::open(filename, O_WRONLY | O_CREAT, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    if(fd < 0) {
        return false;
    }

    if( ::fchmod(fd, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) < 0) {
        return false;
    }

    const char* l1 = {"[Unit]\r\nDescription="};
    if (::write(fd, l1, strlen(l1)) < 0) {
        return false;
    }

    if (::write(fd, description, strlen(description)) < 0) {
        return false;
    }

    const char* l2 = {"\r\nAfter=syslog.target local-fs.target network.target\r\n[Service]\r\nType=simple\r\nRestart=always\r\nRestartSec=5\r\nExecStart="};
    if (::write(fd, l2, strlen(l2)) < 0) {
        return false;
    }

    if (::write(fd, exec, strlen(exec)) < 0) {
        return false;
    }

    const char* l3 = {"\r\n[Install]\r\nWantedBy=multi-user.target\r\n"};
    if (::write(fd, l3, strlen(l3)) < 0) {
        return false;
    }

    if(strlen(extras) > 0) {
        if (::write(fd, extras, strlen(extras)) < 0) {
            return false;
        }
    }

    if (::close(fd) < 0) {
        return false;
    }

    return true;
}

#endif

