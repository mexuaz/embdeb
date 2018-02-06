
/*
 * gpio.h
 *
 *  Created on: Aug 14, 2015
 *      Author: Amir Mehrafsa
 */


#include <sys/types.h>  // stat().
#include <sys/stat.h>   // stat().

#include <cstring> // strlen() strcmp()
#include <stdexcept> // runtime_error() isspace()

#include <fcntl.h>  // open() O_RDONLY ...
#include <unistd.h> // lseek() read() write() close()
#include <sys/epoll.h> // struct epoll_event
#include "gpio.h"



#define GPIO_DIR_INPUT "in"
#define GPIO_DIR_OUTPUT "out"
#define GPIO_EDGE_NONE "none"
#define GPIO_EDGE_RISING "rising"
#define GPIO_EDGE_FALLING "falling"
#define GPIO_EDGE_BOTH "both"

#define GPIOFILE_EXPORT "/export"
#define GPIOFILE_UNEXPORT "/unexport"
#define GPIOFILE_DIRECTION "/direction"
#define GPIOFILE_VALUE "/value"
#define GPIOFILE_EDGE "/edge"

#define GPIO_BASE "/sys/class/gpio"
#define GPIO_EXPORT GPIO_BASE GPIOFILE_EXPORT
#define GPIO_UNEXPORT GPIO_BASE GPIOFILE_UNEXPORT

using namespace amex;


bool gpio::directory_exist(const char* name) {
    struct stat st;
    if(stat(name, &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    return false;
}

char* gpio::trim(char *str) {
  char *end;

  // Trim leading space
  while(isspace(static_cast<unsigned char>(*str))) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace(static_cast<unsigned char>(*end))) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}


gpio::gpio(int nIO)
    : m_io(nIO)
    , m_fdValue(-1)
    , m_fdDirection(-1)
    , m_fdEdge(-1)
    , m_epollEvent(0)
    , m_epoll(-1)
{

}

gpio::~gpio( void )
{
	if(this->exported()) {
		this->set_unexport();
	}
}

int gpio::io( void ) const
{
    return m_io;
}

void gpio::set_export(bool inerrupt_mode /*= false*/)
{
    int fd = -1; // variable for storing file descriptor

    // export if it is not exported yet
    if(!exported()) {
        fd = ::open(GPIO_EXPORT, O_WRONLY);
        if(fd < 0) {
            char buff[128] = {0};
            sprintf(buff, "Failed to open export: %s", strerror(errno));
            throw std::runtime_error(buff);
        }

        char strio[4] = {0};
        sprintf(strio, "%d", m_io);

        if(::write(fd, strio, strlen(strio)) < 0) {
            char buff[128] = {0};
            sprintf(buff, "Failed to write export: %s", strerror(errno));
            throw std::runtime_error(buff);
        }

        ::close(fd);

        // block the executio untill gpio get exported
        while(!exported());
    }



    char fldir[64] = {0};
    sprintf(fldir, GPIO_BASE "/gpio%d" GPIOFILE_DIRECTION, m_io);
    m_fdDirection = ::open(fldir, O_RDWR);
    if(m_fdDirection < 0) {
        char buff[128] = {0};
        sprintf(buff, "Failed to open direction: %s", strerror(errno));
        throw std::runtime_error(buff);
    }


    char fledge[64] = {0};
    sprintf(fledge, GPIO_BASE "/gpio%d" GPIOFILE_EDGE, m_io);
    m_fdEdge = ::open(fledge, O_RDWR);
    if(m_fdEdge < 0) {
        char buff[128] = {0};
        sprintf(buff, "Failed to open edge: %s", strerror(errno));
        throw std::runtime_error(buff);
    }




    if(inerrupt_mode) {
        this->set_direction(dir_t::in);
        this->set_edge(edge_t::rising);
        this->epoll_for_edge_create();
    } else {
        char flval[64] = {0};
        sprintf(flval, GPIO_BASE "/gpio%d" GPIOFILE_VALUE, m_io);
        m_fdValue = ::open(flval, O_RDWR);
        if(m_fdValue < 0) {
            char buff[128] = {0};
            sprintf(buff, "Failed to open value: %s", strerror(errno));
            throw std::runtime_error(buff);
        }
    }

}
void gpio::set_unexport( void )
{

    if(!exported()) {
        return;
    }
	/*
	 * If there are opened files inside exported gpio directory
	 * they should be closed
	 */

    if(m_fdDirection > -1) {
        close(m_fdDirection);
        m_fdDirection = -1;
    }

    if(m_fdEdge > -1) {
        close(m_fdEdge);
        m_fdEdge = -1;
	}


    if(m_fdValue > -1) {
        ::close(m_fdValue);
        m_fdValue = -1;
    }

    if(m_epoll > -1) {
        m_epoll = -1;
	}


    int fd = ::open(GPIO_UNEXPORT, O_WRONLY);
    if(fd < 0) {
        char buff[128] = {0};
        sprintf(buff, "Failed to open unexport: %s", strerror(errno));
        throw std::runtime_error(buff);
    }

    char strio[4] = {0};
    sprintf(strio, "%d", m_io);

    if(::write(fd, strio, strlen(strio)) < 0) {
        char buff[128] = {0};
        sprintf(buff, "Failed to write unexport: %s", strerror(errno));
        throw std::runtime_error(buff);
    }

    ::close(fd);
}

bool gpio::exported( void ) const
{
    char fl[64] = {0};
    sprintf(fl, GPIO_BASE "/gpio%d", m_io);

    if(directory_exist(fl)) {
		return true;
	}

	return false;
}

void gpio::set_direction(dir_t dir /*= dir_t::out*/) {

    if(m_fdDirection < 0) {
        throw std::runtime_error("File descriptor is not correct, set_export call is needed");
    }

    if(::lseek(m_fdDirection, 0L, SEEK_SET) < 0) {
        char buff[128] = {0};
        sprintf(buff, "Failed to seek direction: %s", strerror(errno));
        throw std::runtime_error(buff);
    }

    ssize_t ret = -1;
	switch(dir) {
    case dir_t::out:
        ret = ::write(m_fdDirection, GPIO_DIR_OUTPUT, strlen(GPIO_DIR_OUTPUT));
		break;
    case dir_t::in:
        ret = ::write(m_fdDirection, GPIO_DIR_INPUT, strlen(GPIO_DIR_INPUT));
		break;
	}

    if(ret < 0) {
        char buff[128] = {0};
        sprintf(buff, "Failed to write direction: %s", strerror(errno));
        throw std::runtime_error(buff);
    }
}


gpio::dir_t gpio::direction( void ) const
{
    if(m_fdDirection < 0) {
        throw std::runtime_error("File descriptor is not correct, set_export call is needed");
    }

    if(::lseek(m_fdDirection, 0L, SEEK_SET) < 0) {
        char buffe[128] = {0};
        sprintf(buffe, "Failed to seek direction: %s", strerror(errno));
        throw std::runtime_error(buffe);
    }

    char buff[4] = {0};
    ssize_t ret = ::read(m_fdDirection, buff, 5);
    if(ret < 0) {
        char buffe[128] = {0};
        sprintf(buffe, "Failed to read direction: %s", strerror(errno));
        throw std::runtime_error(buffe);
    }

    trim(buff);

    if(!strcmp(buff, GPIO_DIR_OUTPUT)) {
        return dir_t::out;
    } else if(!strcmp(buff, GPIO_DIR_INPUT)) {
        return dir_t::in;
	} else {
        char buffe[128] = {0};
        sprintf(buffe, "Undefined direction value: %s", buff);
        throw std::runtime_error(buffe);
	}
}

void gpio::set_value(bool val /*= true*/) {
    if(m_fdValue < 0) {
        throw std::runtime_error("File descriptor is not correct, set_export call is needed");
    }

    if(::lseek(m_fdValue, 0L, SEEK_SET) < 0) {
        char buff[128] = {0};
        sprintf(buff, "Unable to seek value: %s", strerror(errno));
        throw std::runtime_error(buff);
    }

    ssize_t ret = ::write(m_fdValue, val ? "1" : "0", 1);
    if(ret < 0) {
        char buff[128] = {0};
        sprintf(buff, "Failed to write value: %s", strerror(errno));
        throw std::runtime_error(buff);
    }
}

bool gpio::value( void ) const {

    if(m_fdValue < 0) {
        throw std::runtime_error("File descriptor is not correct, set_export call is needed");
    }

    if(::lseek(m_fdValue, 0L, SEEK_SET) < 0) {
        char buffe[128] = {0};
        sprintf(buffe, "Unable to seek value: %s", strerror(errno));
        throw std::runtime_error(buffe);
    }

    char buff[4] = {0};
    ssize_t ret = ::read(m_fdValue, buff, 5);
    if(ret < 0) {
        char buffe[128] = {0};
        sprintf(buffe, "Failed to read value: %s", strerror(errno));
        throw std::runtime_error(buffe);
    }

    trim(buff);

    if(!strcmp(buff, "1")) {
        return true;
    } else if(!strcmp(buff, "0")) {
        return false;
    } else {
        char buffe[128] = {0};
        sprintf(buffe, "Undefined value: %s", strerror(errno));
        throw std::runtime_error(buffe);
    }

}

void gpio::set_edge( edge_t edge /*= edge_t::rising*/ ) {

    if(m_fdEdge < 0) {
        throw std::runtime_error("File descriptor is not correct, set_export call is needed");
    }

    if(::lseek(m_fdEdge, 0L, SEEK_SET) < 0) {
        char buff[128] = {0};
        sprintf(buff, "Unable to seek edge: %s", strerror(errno));
        throw std::runtime_error(buff);
    }

    ssize_t ret = -1;
    switch(edge) {
    case edge_t::none:
        ret = ::write(m_fdEdge, GPIO_EDGE_NONE, strlen(GPIO_EDGE_NONE));
        break;
    case edge_t::rising:
        ret = ::write(m_fdEdge, GPIO_EDGE_RISING, strlen(GPIO_EDGE_RISING));
        break;
    case edge_t::falling:
        ret = ::write(m_fdEdge, GPIO_EDGE_FALLING, strlen(GPIO_EDGE_FALLING));
        break;
    case edge_t::both:
        ret = ::write(m_fdEdge, GPIO_EDGE_BOTH, strlen(GPIO_EDGE_BOTH));
        break;
    }

    if(ret < 0) {
        char buff[128] = {0};
        sprintf(buff, "Failed to write edge: %s", strerror(errno));
        throw std::runtime_error(buff);
    }
}

gpio::edge_t gpio::edge( void ) const {

    if(m_fdEdge < 0) {
        throw std::runtime_error("File descriptor is not correct, set_export call is needed");
    }

    if(::lseek(m_fdEdge, 0L, SEEK_SET) < 0) {
        char buffe[128] = {0};
        sprintf(buffe, "Failed to seek edge: %s", strerror(errno));
        throw std::runtime_error(buffe);
    }

    char buff[8] = {0};
    ssize_t ret = ::read(m_fdEdge, buff, 5);
    if(ret < 0) {
        char buffe[128] = {0};
        sprintf(buffe, "Failed to read edge: %s", strerror(errno));
        throw std::runtime_error(buffe);
    }

    trim(buff);

    if(strcmp(buff, GPIO_EDGE_NONE) == 0) {
        return edge_t::none;
    } else if(strcmp(buff, GPIO_EDGE_RISING) == 0) {
        return edge_t::rising;
    } else if(strcmp(buff, GPIO_EDGE_FALLING) == 0) {
        return edge_t::falling;
    } else if(strcmp(buff, GPIO_EDGE_BOTH) == 0) {
        return edge_t::both;
    } else {
        char buffe[128] = {0};
        sprintf(buffe, "Undefined edge value: %s", buff);
        throw std::runtime_error(buffe);
    }

}

void gpio::epoll_for_edge_create( void )
{
    if(m_fdValue > 0) {
        ::close(m_fdValue);
        m_fdValue = -1;
	}

    char flval[64] = {0};
    sprintf(flval, GPIO_BASE "/gpio%d" GPIOFILE_VALUE, m_io);
    m_fdValue = ::open(flval, O_RDONLY | O_NONBLOCK);
    if(m_fdValue < 0) {
        char buff[128] = {0};
        sprintf(buff, "Epoll, failed to open value: %s", strerror(errno));
        throw std::runtime_error(buff);
    }


    m_epoll = epoll_create(1);
    if (m_epoll == -1) {
        close(m_fdValue);
        m_fdValue = -1;
        char buff[128] = {0};
        sprintf(buff, "Epoll, failed to create handle: %s", strerror(errno));
        throw std::runtime_error(buff);
	}

	// events = read operation | edge triggered | urgent data
    m_epollEvent = EPOLLIN | EPOLLET | EPOLLPRI;

    struct epoll_event epollEvent;
    epollEvent.events = m_epollEvent;
    epollEvent.data.fd = m_fdValue;  // attach the file descriptor to epoll event

	//Register the file descriptor on the epoll instance, see: man epoll_ctl
    if (epoll_ctl(m_epoll, EPOLL_CTL_ADD, m_fdValue, &epollEvent) == -1) {
        close(m_fdValue);
        m_fdValue = -1;
        m_epoll = -1;
        char buff[128] = {0};
        sprintf(buff, "Epoll: failed to configure: %s", strerror(errno));
        throw std::runtime_error(buff);
	}

}

void gpio::epoll_for_edge_wait( int timeout /*= -1*/ )
{
    if(m_epoll == -1) {
        char buff[128] = {0};
        sprintf(buff, "Epoll, Not created yet: %s", strerror(errno));
        throw std::runtime_error(buff);
	}

    struct epoll_event epollEvent;
    epollEvent.events = m_epollEvent;
    epollEvent.data.fd = m_fdValue;  // attach the file descriptor to epoll event

    if (epoll_wait(m_epoll, &epollEvent, 1, timeout) == -1) {
        close(m_fdValue);
        m_fdValue = -1;
        m_epoll = -1;
        char buff[128] = {0};
        sprintf(buff, "Epoll, waiting failed: %s", strerror(errno));
        throw std::runtime_error(buff);
	}
}


