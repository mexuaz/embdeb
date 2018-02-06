/*
 * spibus.h
 *
 *  Created on: Aug 25, 2015
 *      Author: Amir Mehrafsa
 */
#ifndef SPIBUS_H_
#define SPIBUS_H_

#include "bus.h"

#include <cstddef> // size_t

namespace amex {

    class spi : public bus {

    public:

        enum class mode_t {
            m0 = 0,   // Low at idle (CPOL:0), capture on rising clock edge (CPHA:0)
            idle_low_cedge_rising = 0, // Clock polarity = Low, Clock Phase = Low
            m1 = 1,   // Low at idle(CPOL:0), capture on falling clock edge (CPHA:1)
            idle_low_cedge_falling = 1, // Clock polarity = Low, Clock Phase = High
            m2 = 2,   // High at idle(CPOL:1), capture on falling clock edge (CPHA:0)
            idle_high_cedge_falling = 2, // Clock polarity = High, Clock Phase = Low
            m3 = 3,    // High at idle(CPOL:1), capture on rising clock edge (CPHA:1)
            idle_high_cedge_rising = 3 // Clock polarity = High, Clock Phase = High
        };

        enum class bit_justification_t {
            msb_first,
            lsb_first
        };

    public:
        /*
         * The constructor for the spibus
         */
        spi();





    public:
        /*
         * This method opens the file connection to the SPI device.
         * @param bus The SPI bus number X (first digit after spidevX.Y)
         * @param device The device on the bus Y (second digit after spidevX.Y)
         */
        void open(
                unsigned int bus, unsigned int device,
                mode_t mode = mode_t::idle_high_cedge_rising,
                bit_justification_t bj = bit_justification_t::msb_first,
                uint32_t speed = 500000,
                uint8_t bpw = 8);


        /*
         *   Set the mode of the SPI bus
         *   @param mode the enumerated SPI mode
         */
        void setMode(mode_t mode);
        mode_t mode( void ) const;


        void setBitJustification(bit_justification_t bj);
        bit_justification_t bitJustification( void ) const;

        /*
         *   Set the speed of the SPI bus
         *   @param speed the speed in Hz
         */
        void setSpeed(uint32_t speed);
        uint32_t speed( void ) const;

        /*
         *   Set the number of bits per word of the SPI bus
         *   @param bits the number of bits per word
         */
        void setBitsPerWord(uint8_t bpw);
        uint8_t bitsPerWord( void ) const;

        /*
         * Generic method to transfer data to and from the SPI device. It is used by the
         * following methods to read and write registers.
         * @param send The array of data to send to the SPI device
         * @param receive The array of data to receive from the SPI device
         * @param length The length of the arrays for send and receive arguments
         * throws on failure
         */
        void transfer(uint8_t* send, uint8_t* receive, size_t length);

        void transfer(uint16_t* send, uint16_t* receive, size_t length);

        void transfer(uint32_t* send, uint32_t* receive, size_t length);

        /*
         * Transfers only one byte and receives one byte
         * @param send byte to send
         * @return byte received
         */
        uint8_t transferByte(uint8_t send);
        uint16_t transferHalfword(uint16_t send);
        uint32_t transferWord(uint32_t send);

    }; // end of class spi

} // end of namespace amex


#endif /* SPIBUS_H_ */
