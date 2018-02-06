#include "bus.h"

#include <cstring>      // strerror()
#include <stdexcept>    // std::runtime_error <string>
#include <unistd.h>     // ::close()

using namespace amex;

bus::bus()
    : m_bus(0)
    , m_device(0)
    , m_file(-1) {

}

bus::~bus() {
    close();
}

void bus::close() {
    if(isOpen()) {
        if(::close(this->m_file) < 0) {
            std::string str("I2C close failed: ");
            str.append(strerror(errno));
            throw std::runtime_error(str.c_str());
        }
        this->m_bus = 0;
        this->m_device = 0;
        this->m_file = -1;
    }
}

bool bus::isOpen() const {
    return this->m_file > 0;
}

uint16_t bus::ByteArray2Int16(uint8_t lowByte,
                              uint8_t highByte,
                              bool little_endian) {
    uint16_t value;
    if(little_endian) {
        value = static_cast<uint16_t>(highByte << 8)
                | static_cast<uint16_t>(lowByte);
    } else {
        value = static_cast<uint16_t>(lowByte << 8)
                | static_cast<uint16_t>(highByte);
    }
    return value;
}

uint32_t bus::ByteArray2Int32(uint8_t lowByte,
                              uint8_t midByte,
                              uint8_t highByte,
                              bool little_endian) {
    uint32_t value = 0;
    if(little_endian) {
        value = static_cast<uint32_t>(highByte << 16)
                | static_cast<uint32_t>(midByte << 8)
                | static_cast<uint32_t>(lowByte);
    } else {
        value = static_cast<uint32_t>(lowByte << 16)
                | static_cast<uint32_t>(midByte << 8)
                | static_cast<uint32_t>(highByte);
    }
    return value;
}

uint32_t bus::ByteArray2Int32(uint8_t lowByte,
                              uint8_t byte1,
                              uint8_t byte2,
                              uint8_t highByte,
                              bool little_endian) {
    uint32_t value;
    if(little_endian) {
        value = static_cast<uint32_t>(highByte << 24)
                | static_cast<uint32_t>(byte2 << 16)
                | static_cast<uint32_t>(byte1 << 8)
                | static_cast<uint32_t>(lowByte);
    } else {
        value = static_cast<uint32_t>(lowByte << 24)
                | static_cast<uint32_t>(byte1 << 16)
                | static_cast<uint32_t>(byte2 << 8)
                | static_cast<uint32_t>(highByte);
    }
    return value;
}

void bus::Integer2ByteArray(uint16_t value, uint8_t bytes[], bool little_endian) {
    if(little_endian) {
        bytes[1] = static_cast<uint8_t>((value >> 8) & 0xFF); // High word
        bytes[0] = static_cast<uint8_t>(value & 0xFF); // Low word
    } else {
        bytes[0] = static_cast<uint8_t>((value >> 8) & 0xFF); // High word
        bytes[1] = static_cast<uint8_t>(value & 0xFF); // Low word
    }
}

void bus::Integer2ByteArray(uint32_t value, uint8_t bytes[], bool little_endian) {
    if(little_endian) {
        bytes[3] = static_cast<uint8_t>((value >> 24) & 0xFF); // High word
        bytes[2] = static_cast<uint8_t>((value >> 16) & 0xFF);
        bytes[1] = static_cast<uint8_t>((value >> 8) & 0xFF);
        bytes[0] = static_cast<uint8_t>(value & 0xFF); // Low word
    } else {
        bytes[0] = static_cast<uint8_t>((value >> 24) & 0xFF); // High word
        bytes[1] = static_cast<uint8_t>((value >> 16) & 0xFF);
        bytes[2] = static_cast<uint8_t>((value >> 8) & 0xFF);
        bytes[3] = static_cast<uint8_t>(value & 0xFF); // Low word
    }
}
