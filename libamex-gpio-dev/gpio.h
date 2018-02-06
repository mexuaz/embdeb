/*
 * gpio.h
 *
 *  Created on: Aug 14, 2015
 *      Author: Amir Mehrafsa
 */


#ifndef GPIO_H
#define GPIO_H

namespace amex {
    class gpio {

        int m_io;

        int m_fdValue;          // file descriptor for gpio value (this is also used in epoll)
        int m_fdDirection;      // file descriptor for gpio direction
        int m_fdEdge;           // file descriptor for gpio edge
        unsigned m_epollEvent;  // keeps epoll event values
        int m_epoll;            // handle for gpio epoll

    public:

        enum class dir_t {
            out,
            in
        };

        enum class edge_t {
            none,
            rising,
            falling,
            both
        };

    private:
        static bool directory_exist(const char* name);

        static char* trim(char *str);


    public:
        gpio(int nIO);
        ~gpio( void );


        /*
         * Returns the gpio number in integer form.
         */
        int io( void ) const;


        /*
         * export the gpio pin
         */
        void set_export( bool inerrupt_mode = false );

        /*
         * unexport the pin
         */
        void set_unexport( void );

        /*
         * verifies if the pin is already exported or not
         */
        bool exported( void ) const;

        /*
         * Sets the value of gpio direction file.
         */
        void set_direction(dir_t dir = dir_t::out);

        /*
         * Reads the content of gpio direction file.
         */
        dir_t direction( void ) const;

        /*
         * Sets the value of gpio value file.
         */
        void set_value(bool val = true);


        /*
         * Reads the content of gpio value file and returns the appropriate value.
         */
        bool value( void ) const;

        /*
         * Sets the value of gpio edge file.
         */
        void set_edge( edge_t edge = edge_t::rising );

        /*
         * Reads the content of gpio edge file and returns the appropriate value.
         */
        edge_t edge( void ) const;

    private:
        /*
         * Blocking Poll - based on the epoll socket code in the epoll man page
         * Note1: while using epoll system if you need to read the gpio value file
         * content use epoll_read_value method and do not use value method since they
         * use different file descriptors.
         */
        void epoll_for_edge_create( void );

    public:
        /*
         * Wait for edge interrupt, the interrupt mode should be set
         * by setEdge method before calling this function otherwise it
         * will fail.
         * Note1: when using polling you should ignore the first trigger
         * and count the triggers up to the 2nd one.
         */
        void epoll_for_edge_wait( int timeout = -1 );


    };
}






#endif // GPIO_H
