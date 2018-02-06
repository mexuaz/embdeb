#ifndef I2CBUS_H_
#define I2CBUS_H_

#include "bus.h"

#include <cstddef> // size_t

namespace amex {
    /**
     * @class I2CBus
     * @brief Generic I2C Device class that can be used to connect to any type of I2C device and read or write to its registers
     */
    class i2c : public bus {


        /**
         * Write a single value to the I2C device. Used to set up the device to read from a
         * particular address.
         * @param value the value to write to the device
         * @return 1 on failure to write, 0 on success.
         */
        bool write(uint8_t value);

    public:

        /**
         * Constructor for the I2CBus class.
         */
        i2c();

        /**
         * Open a connection to an I2C device
         * Opens a file handle to the I2C device, which is destroyed when the destructor is called
         * @param bus The bus number. Usually 0 or 1 on the BBB
         * @param device The device ID on the bus.
         * @throws on failure.
         * @return true on success.
         */
        bool open(unsigned int bus, unsigned int device);





        /**
         * Read a single register value from the address on the device.
         * @param registerAddress the address to read from
         * @return the byte value at the register address.
         */
        uint8_t readRegister(uint8_t registerAddress);


        uint16_t readHalfword(uint8_t registerAddress, bool little_endian = true);
        uint32_t read24(uint8_t registerAddress, bool little_endian = true);
        uint32_t readWord(uint8_t registerAddress, bool little_endian = true);

        /**
         * Method to read a number of registers from a single device. This is much more efficient than
         * reading the registers individually. The from address is the starting address to read from, which
         * defaults to 0x00.
         * @param fromAddress the starting address to read from
         * @param nbytes the number of registers (bytes) to read from the device
         * @param buff a pointer of type uint8_t* that points to the first element in the block of registers
         */
        bool readRegisters(uint8_t fromAddress, uint8_t* buff, size_t nbytes);

        /**
         * Write a single byte value to a single register.
         * @param registerAddress The register address
         * @param value The value to be written to the register
         * @return true on success, throws on failure to write, .
         */
        bool writeRegister(uint8_t registerAddress, uint8_t value);

        /**
         * Method to dump the registers to the standard output. It inserts a return character after every
         * 16 values and displays the results in hexadecimal to give a standard output using the HEX() macro
         * that is defined at the top of this file. The standard output will stay in hexadecimal format, hence
         * the call on the last like.
         * @param number the total number of registers to dump, defaults to 0xff
         */
        void debugDumpRegisters(unsigned int number = 0xff);

    }; // end of class i2c
}   // end of namespace amex





#endif /* I2CBUS_H_ */
