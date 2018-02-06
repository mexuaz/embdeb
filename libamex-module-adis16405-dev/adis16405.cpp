#include "adis16405.h"

#include <cstdio>               // fprintf
#include <stdexcept>            // std::runtime_error() <string>

#define ONLYREAD(X) (X & 0x7F)
#define READWRITE(X) (X &0x80)

using namespace amex;


int32_t adis16405::from_16bit_2s_complement(uint16_t value)
{
    /*
     *  if first bit is 1, it is a negative number with all other bits in 2's complement
     *  extract negative part by subtracting off the 0x8000
     *  otherwise the lower bits are just the positive integer value
     */
    int32_t sval = static_cast<int32_t>(0x0000FFFF & value);
    int32_t result = (0x8000 & sval ? (0x7FFF & sval)-0x8000 : sval);
    return result;
}

int32_t adis16405::from_14bit_2s_complement(uint16_t value)
{
    int32_t sval = static_cast<int32_t>(0x00003FFF & value);
    int32_t result = (0x2000 & sval ? (0x1FFF & sval)-0x2000 : sval);
    return result;
}

int32_t adis16405::from_12bit_2s_complement(uint16_t value)
{
    int32_t sval = static_cast<int32_t>(0x00000FFF & value);
    int32_t result = (0x800 & sval ? (0x7FF & sval)-0x800 : sval);
    return result;
}

int32_t adis16405::from_2s_complement(uint16_t value, uint8_t nbit)
{
    /*
     *  IF first bit is 1{(1<<nbit) & sval}, it is a negative number with all other bits in 2's complement
     *  extract negative part by subtracting off the value with 1 in msb (1<<nbit)
     *  otherwise the lower bits are just the positive integer value
     */
    uint32_t mask = 0xFFFFFFFF >> (32 - nbit);
    int32_t sval = static_cast<int32_t>(mask & value);
    int32_t result = ((1<<nbit) & sval ? (static_cast<int32_t>(mask>>1) & sval)-(1<<nbit) : sval);
    return result;
}

float adis16405::decode_14bit(uint16_t value, float scale)
{
    return from_14bit_2s_complement(value)*scale;
}

float adis16405::decode_12bit(uint16_t value, float scale)
{
    return from_12bit_2s_complement(value)*scale;
}

bool adis16405::validate_device()
{
    // Product identifier register 0x56 value is 0x4015
    m_spi.transferHalfword(spi::ByteArray2Int16(0x00, ONLYREAD(MemoryMap::PRODUCT_ID), true));
    m_spi.transferHalfword(spi::ByteArray2Int16(0x00, ONLYREAD(MemoryMap::PRODUCT_ID), true));
    uint16_t rec = m_spi.transferHalfword(spi::ByteArray2Int16(0x00, ONLYREAD(MemoryMap::PRODUCT_ID), true));
    if(rec == 0x4015) {
        return true;
    } else {
        return false;
    }
}

float adis16405::power_supply()
{
    m_spi.transferHalfword(spi::ByteArray2Int16(0x00, ONLYREAD(MemoryMap::SUPPLY_OUT), true));
    uint16_t rec = m_spi.transferHalfword(0x0000);
    return adis16405::decode_14bit(rec, 2.418f)/1000.f;
}

float adis16405::gyroscope_x()
{
    m_spi.transferHalfword(spi::ByteArray2Int16(0x00, ONLYREAD(MemoryMap::XGYRO_OUT), true));
    uint16_t rec = m_spi.transferHalfword(0x0000);
    return adis16405::decode_14bit(rec, 0.05f);
}

float adis16405::gyroscope_y()
{
    m_spi.transferHalfword(spi::ByteArray2Int16(0x00, ONLYREAD(MemoryMap::YGYRO_OUT), true));
    uint16_t rec = m_spi.transferHalfword(0x0000);
    return adis16405::decode_14bit(rec, 0.05f);
}

float adis16405::gyroscope_z()
{
    m_spi.transferHalfword(spi::ByteArray2Int16(0x00, ONLYREAD(MemoryMap::ZGYRO_OUT), true));
    uint16_t rec = m_spi.transferHalfword(0x0000);
    return adis16405::decode_14bit(rec, 0.05f);
}

float adis16405::accelerometer_x()
{
    m_spi.transferHalfword(spi::ByteArray2Int16(0x00, ONLYREAD(MemoryMap::XACCL_OUT), true));
    uint16_t rec = m_spi.transferHalfword(0x0000);
    return adis16405::decode_14bit(rec, 3.33f)/1000.f;
}

float adis16405::accelerometer_y()
{
    m_spi.transferHalfword(spi::ByteArray2Int16(0x00, ONLYREAD(MemoryMap::YACCL_OUT), true));
    uint16_t rec = m_spi.transferHalfword(0x0000);
    return adis16405::decode_14bit(rec, 3.33f)/1000.f;
}

float adis16405::accelerometer_z()
{
    m_spi.transferHalfword(spi::ByteArray2Int16(0x00, ONLYREAD(MemoryMap::ZACCL_OUT), true));
    uint16_t rec = m_spi.transferHalfword(0x0000);
    return adis16405::decode_14bit(rec, 3.33f)/1000.f;
}

float adis16405::magnetometer_x()
{
    m_spi.transferHalfword(spi::ByteArray2Int16(0x00, ONLYREAD(MemoryMap::XMAGN_OUT), true));
    uint16_t rec = m_spi.transferHalfword(0x0000);
    return adis16405::decode_14bit(rec, 0.5f)/1000.f;
}

float adis16405::magnetometer_y()
{
    m_spi.transferHalfword(spi::ByteArray2Int16(0x00, ONLYREAD(MemoryMap::YMAGN_OUT), true));
    uint16_t rec = m_spi.transferHalfword(0x0000);
    return adis16405::decode_14bit(rec, 0.5f)/1000.f;
}

float adis16405::magnetometer_z()
{
    m_spi.transferHalfword(spi::ByteArray2Int16(0x00, ONLYREAD(MemoryMap::ZMAGN_OUT), true));
    uint16_t rec = m_spi.transferHalfword(0x0000);
    return adis16405::decode_14bit(rec, 0.5f)/1000.f;
}

float adis16405::temperature()
{
    m_spi.transferHalfword(spi::ByteArray2Int16(0x00, ONLYREAD(MemoryMap::TEMP_OUT), true));
    uint16_t rec = m_spi.transferHalfword(0x0000);
    return adis16405::decode_12bit(rec, 0.14f)+25;
}

float adis16405::adc()
{
    m_spi.transferHalfword(spi::ByteArray2Int16(0x00, ONLYREAD(MemoryMap::AUX_DAC), true));
    uint16_t rec = m_spi.transferHalfword(0x0000);
    return adis16405::decode_14bit(rec, 806.0f)/1e6f;
}

void adis16405::data_burst(adis16405::adisinfo_t &info)
{
    /* Initiate burst read */
    m_spi.transferHalfword(0x3E00);

    /* Read all data from the burst read at put in struct */
    info.power_supply       = adis16405::decode_14bit(m_spi.transferHalfword(0x0000), 2.418f)/1000.f;
    info.gyroscope_x        = adis16405::decode_14bit(m_spi.transferHalfword(0x0000), 0.050f);
    info.gyroscope_y        = adis16405::decode_14bit(m_spi.transferHalfword(0x0000), 0.050f);
    info.gyroscope_z        = adis16405::decode_14bit(m_spi.transferHalfword(0x0000), 0.050f);
    info.accelerometer_x    = adis16405::decode_14bit(m_spi.transferHalfword(0x0000), 3.330f)/1000.f;
    info.accelerometer_y    = adis16405::decode_14bit(m_spi.transferHalfword(0x0000), 3.330f)/1000.f;
    info.accelerometer_z    = adis16405::decode_14bit(m_spi.transferHalfword(0x0000), 3.330f)/1000.f;
    info.magnetometer_x     = adis16405::decode_14bit(m_spi.transferHalfword(0x0000), 0.500f)/1000.f;
    info.magnetometer_y     = adis16405::decode_14bit(m_spi.transferHalfword(0x0000), 0.500f)/1000.f;
    info.magnetometer_z     = adis16405::decode_14bit(m_spi.transferHalfword(0x0000), 0.500f)/1000.f;
    info.temperature        = adis16405::decode_12bit(m_spi.transferHalfword(0x0000), 0.140f)+25;
    info.aux_adc            = adis16405::decode_12bit(m_spi.transferHalfword(0x0000), 806.0f)/1e6f;
}



void adis16405::reset_software()
{
    m_spi.transferHalfword(0xBE80);
}

void adis16405::reset_factory()
{
    m_spi.transferHalfword(0xBE02);
}

adis16405::adis16405(unsigned int bus, unsigned int dev) {

    // SPI settings for AIDS16405: 500000, MSBFIRST, CPOL: 1, CPHA 1 => MODE 3, 16bpw
    m_spi.open(bus, dev,
               spi::mode_t::m3,
               spi::bit_justification_t::msb_first,
               500000, 16);

#if(VALIDATEDEVICES)
    if(!validate_device()) {
        throw std::runtime_error("AIDS16405 ID is not correct.");
    }
#endif
}

adis16405::~adis16405()
{
    if(m_spi.isOpen()) {
        m_spi.close();
    }
}
