#include "i2cbus.h"


#include <iostream>             // cout endl
#include <iomanip>              // setw setfill
#include <fcntl.h>              // ::open() O_RDWR
#include <unistd.h>             // ::read() ::write()
#include <sys/ioctl.h>          // ioctl()
#include <linux/spi/spidev.h>   // spi_ioc_transfer SPI_IOC_MESSAGE
#include <linux/i2c-dev.h>      // I2C_SLAVE
#include <cerrno>               // errno
#include <stdexcept>            // std::runtime_error() <string>
#include <cstring>              // memset() strerror()

using namespace amex;

#define HEX(x) std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(x)

#define BUS_I2C "/dev/i2c-"

bool i2c::write(uint8_t value) {
   if (::write(this->m_file, &value, 1)!=1){
      std::string str("I2C byte write failed: ");
      str.append(strerror(errno));
      throw std::runtime_error(str.c_str());
   }
   return true;
}

i2c::i2c()
    : bus() {
}

bool i2c::open(unsigned int bus, unsigned int device) {
    this->m_bus = bus;
    this->m_device = device;
    std::string str = BUS_I2C + std::to_string(bus);

   if((this->m_file=::open(str.c_str(), O_RDWR)) < 0) {
       std::string str("I2C open bus failed: ");
       str.append(strerror(errno));
       throw std::runtime_error(str.c_str());
   }
   if(ioctl(this->m_file, I2C_SLAVE, this->m_device) < 0) {
       std::string str("I2C connect to slave failed: ");
       str.append(strerror(errno));
       throw std::runtime_error(str.c_str());
   }
   return true;
}




uint8_t i2c::readRegister(uint8_t registerAddress) {
   this->write(registerAddress);
   uint8_t buff;
   if(::read(this->m_file, &buff, 1) != 1) {
       std::string str("I2C byte read failed: ");
       str.append(strerror(errno));
       throw std::runtime_error(str.c_str());
   }
   return buff;
}

uint16_t i2c::readHalfword(uint8_t registerAddress, bool little_endian) {
    uint8_t buff[2] = {0};
    readRegisters(registerAddress, buff, 2);
    return i2c::ByteArray2Int16(buff[0], buff[1], little_endian);
}

uint32_t i2c::read24(uint8_t registerAddress, bool little_endian) {
    uint8_t buff[3] = {0};
    readRegisters(registerAddress, buff, 3);
    return i2c::ByteArray2Int32(buff[0], buff[1], buff[2], little_endian);
}

uint32_t i2c::readWord(uint8_t registerAddress, bool little_endian) {
    uint8_t buff[4] = {0};
    readRegisters(registerAddress, buff, 4);
    return i2c::ByteArray2Int32(buff[0], buff[1], buff[2], buff[3], little_endian);
}

bool i2c::readRegisters(uint8_t fromAddress, uint8_t* buff, size_t nbytes) {
    this->write(fromAddress);
    if(::read(this->m_file, buff, nbytes) != static_cast<ssize_t>(nbytes)) {
        std::string str("I2C buffer read failed: ");
        str.append(strerror(errno));
        throw std::runtime_error(str.c_str());
    }
    return true;
}

bool i2c::writeRegister(uint8_t registerAddress, uint8_t value) {
   uint8_t buffer[2] = {registerAddress, value};
   if(::write(this->m_file, buffer, 2)!=2){
       std::string str("I2C write register failed: ");
       str.append(strerror(errno));
       throw std::runtime_error(str.c_str());
   }
   return true;
}

void i2c::debugDumpRegisters(unsigned int number) {
    std::cout << "Dumping Registers for Debug Purposes:" << std::endl;
    uint8_t* registers = new uint8_t[number];
    if(!registers) {
        std::string str("Insufficient memory: ");
        str.append(strerror(errno));
        throw std::runtime_error(str.c_str());
    }
    if(!this->readRegisters(0xff, registers, number)) {
        return;
    }
    for(unsigned int i=0; i < number; i++){
        std::cout << HEX(registers[i]) << " ";
        if (i%16==15) {
            std::cout << std::endl;
        }
	}
    delete[] registers;
    std::cout << std::dec;
}


