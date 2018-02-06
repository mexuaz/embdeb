#ifndef BUS_H
#define BUS_H

#include <cstdint> // uint#_t, int#_t

/**
 * @class Abstract class for bus implementation
 * @brief I2C and SPI Bus will inherit form this class
 * @author Amir Mehrafsa
 */

#define VALIDATEDEVICES 0

namespace amex {

    class bus {

    protected:
        unsigned int m_bus;    /* bus number */
        unsigned int m_device; /* device number on the bus (slave address for i2c bus) */
        int m_file;            /* file handle to the device */

    public:
        bus();
        virtual ~bus();



        /*
         * Close the file handles and sets a temporary state to -1.
         */
        void close();

        bool isOpen( void ) const;

        static uint16_t ByteArray2Int16(uint8_t lowByte,
                                        uint8_t highByte,
                                        bool little_endian = true);

        static uint32_t ByteArray2Int32(uint8_t lowByte,
                                        uint8_t midByte,
                                        uint8_t highByte,
                                        bool little_endian = true);

        static uint32_t ByteArray2Int32(uint8_t lowByte,
                                        uint8_t byte1,
                                        uint8_t byte2,
                                        uint8_t highByte,
                                        bool little_endian = true);

        static void Integer2ByteArray(uint16_t value,
                                      uint8_t bytes[2], bool little_endian = true);

        static void Integer2ByteArray(uint32_t value,
                                      uint8_t bytes[4], bool little_endian = true);

    };

}

#endif // BUS_H
