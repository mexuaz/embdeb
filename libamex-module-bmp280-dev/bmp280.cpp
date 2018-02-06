#include "bmp280.h"

#include <cmath>                // pow
#include <cstdio>               // fprintf
#include <stdexcept>            // std::runtime_error() <string>

using namespace amex;

uint8_t bmp280::id() {
    return m_i2c.readRegister(MemoryMap::CHIPID | 0x80);
}

void bmp280::set_soft_reset() {
    m_i2c.writeRegister(MemoryMap::SOFTRESET, 0xB6 & ~0x80);
}

void bmp280::set_control() {
    // 1x temperature oversampling, 16x presure oversampling, start normal mode
    m_i2c.writeRegister(MemoryMap::CONTROL, 0x3F & ~0x80);
}

void bmp280::read_coefficients( bmp280_calib_data& calib ) {
    // according to table 17 in datasheet LSB is stored in lower address so it is little endian
    bool little_endian = true;
    calib.dig_T1 = m_i2c.readHalfword(MemoryMap::DIG_T1, little_endian);
    calib.dig_T2 = static_cast<int16_t>(m_i2c.readHalfword(MemoryMap::DIG_T2, little_endian));
    calib.dig_T3 = static_cast<int16_t>(m_i2c.readHalfword(MemoryMap::DIG_T3, little_endian));

    calib.dig_P1 = m_i2c.readHalfword(MemoryMap::DIG_P1);
    calib.dig_P2 = static_cast<int16_t>(m_i2c.readHalfword(MemoryMap::DIG_P2, little_endian));
    calib.dig_P3 = static_cast<int16_t>(m_i2c.readHalfword(MemoryMap::DIG_P3, little_endian));
    calib.dig_P4 = static_cast<int16_t>(m_i2c.readHalfword(MemoryMap::DIG_P4, little_endian));
    calib.dig_P5 = static_cast<int16_t>(m_i2c.readHalfword(MemoryMap::DIG_P5, little_endian));
    calib.dig_P6 = static_cast<int16_t>(m_i2c.readHalfword(MemoryMap::DIG_P6, little_endian));
    calib.dig_P7 = static_cast<int16_t>(m_i2c.readHalfword(MemoryMap::DIG_P7, little_endian));
    calib.dig_P8 = static_cast<int16_t>(m_i2c.readHalfword(MemoryMap::DIG_P8, little_endian));
    calib.dig_P9 = static_cast<int16_t>(m_i2c.readHalfword(MemoryMap::DIG_P9, little_endian));
}

void bmp280::sample_coefficients(bmp280::bmp280_calib_data &calib) {
    calib.dig_T1 = 27504;
    calib.dig_T2 = 26435;
    calib.dig_T3 = -1000;

    calib.dig_P1 = 36477;
    calib.dig_P2 = -10685;
    calib.dig_P3 = 3024;
    calib.dig_P4 = 2855;
    calib.dig_P5 = 140;
    calib.dig_P6 = -7;
    calib.dig_P7 = 15500;
    calib.dig_P8 = -14600;
    calib.dig_P9 = 6000;
}

void bmp280::print_coefficients(const bmp280::bmp280_calib_data &calib) {
    fprintf(stdout, "T1: %u, T2: %+d, T3: %+d, P1: %u, P2: %+d, P3: %+d, P4: %+d, P5: %+d, P6: %+d, P7: %+d, P8: %+d, P9: %+d",
           calib.dig_T1,
           calib.dig_T2,
           calib.dig_T3,

           calib.dig_P1,
           calib.dig_P2,
           calib.dig_P3,
           calib.dig_P4,
           calib.dig_P5,
           calib.dig_P6,
           calib.dig_P7,
           calib.dig_P8,
           calib.dig_P9
           );
}

int32_t bmp280::read_adc_temperature() {
    // MSB is sotred in lower address so it is big endian
    uint32_t u32 = m_i2c.read24(MemoryMap::TEMPDATA | 0x80, false);
    u32 >>= 4;
    return  static_cast<int32_t>(u32);
}

int32_t bmp280::read_adc_presure() {
    // MSB is sotred in lower address so it is big endian
    uint32_t u32 = m_i2c.read24(MemoryMap::PRESSUREDATA | 0x80, false);
    u32 >>= 4;
    return  static_cast<int32_t>(u32);
}



int32_t bmp280::calculate_temperature_fine(const bmp280_calib_data& calib, int32_t adc_T) {
    int32_t var1 = ((((adc_T>>3) - (static_cast<int32_t>(calib.dig_T1)<<1))) * static_cast<int32_t>(calib.dig_T2)) >> 11;
    int32_t var2 = (((((adc_T>>4) - static_cast<int32_t>(calib.dig_T1)) * ((adc_T>>4) - static_cast<int32_t>(calib.dig_T1))) >> 12) * static_cast<int32_t>(calib.dig_T3)) >> 14;
    return var1 + var2;
}

int32_t bmp280::compensate_temperature_s32(int32_t temperature_fine) {
    return (temperature_fine * 5 + 128) >> 8;
}

double bmp280::compensate_temperature(int32_t temperature_fine) {
    return static_cast<double>(compensate_temperature_s32(temperature_fine)) / 100.;

}

uint32_t bmp280::compensate_pressure_u32(const bmp280_calib_data& calib, int32_t temperature_fine, int32_t adc_P) {
    int64_t var1 = static_cast<int64_t>(temperature_fine) - 128000;
    int64_t var2 = var1 * var1 * static_cast<int64_t>(calib.dig_P6);
    var2 = var2 + ((var1*static_cast<int64_t>(calib.dig_P5))<<17);
    var2 = var2 + ((static_cast<int64_t>(calib.dig_P4))<<35);
    var1 = ((var1 * var1 * static_cast<int64_t>(calib.dig_P3))>>8) + ((var1 * static_cast<int64_t>(calib.dig_P2))<<12);
    var1 = ((((static_cast<int64_t>(1))<<47)+var1))*(static_cast<int64_t>(calib.dig_P1))>>33;
    if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
    }
    int64_t p = 1048576-adc_P;
    p = (((p<<31)-var2)*3125)/var1;
    var1 = ((static_cast<int64_t>(calib.dig_P9)) * (p>>13) * (p>>13)) >> 25;
    var2 = ((static_cast<int64_t>(calib.dig_P8)) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + ((static_cast<int64_t>(calib.dig_P7))<<4);
    return static_cast<uint32_t>(p);
}

double bmp280::compensate_pressure(const bmp280_calib_data& calib, int32_t temperature_fine, int32_t adc_P) {
    return compensate_pressure_u32(calib, temperature_fine, adc_P)/256.;
}




bmp280::bmp280(unsigned int bus, unsigned int dev) {

    m_i2c.open(bus, dev);

#if(VALIDATEDEVICES)
    if(id() != product_id) {
        throw std::runtime_error("BMP280 ID is not correct.");
    }
#endif

    // BMP280 ID validated successfully.

    set_soft_reset();

    //sample_coefficients(_bmp280_calib);
    read_coefficients(_bmp280_calib);

    set_control();

}

bmp280::~bmp280() {
    if(m_i2c.isOpen()) {
        m_i2c.close();
    }
}

double bmp280::calculate_altitude(double pressure, double seaLevelhPa /*= 1013.25*/) {
    pressure /= 100;
    return 44330 * (1.0 - pow(pressure / seaLevelhPa, 0.1903));
}

void bmp280::read(double &temperature) {
    int32_t adc_T = read_adc_temperature();

    int32_t T_fine = calculate_temperature_fine(_bmp280_calib, adc_T);

    temperature = compensate_temperature(T_fine);
}

void bmp280::read(double& temperature, double& pressure) {
    int32_t adc_T = read_adc_temperature();
    int32_t adc_P = read_adc_presure();

    int32_t T_fine = calculate_temperature_fine(_bmp280_calib, adc_T);

    temperature = compensate_temperature(T_fine);
    pressure = compensate_pressure(_bmp280_calib, T_fine, adc_P);
}

bool bmp280::validate_datasheet_sample()
{
    bmp280_calib_data calib;
    sample_coefficients(calib);

    int32_t adc_T = 519888;
    int32_t adc_P = 415148;

    int32_t T_fine = calculate_temperature_fine(calib, adc_T); // 128422

    int32_t temperature_s32 = compensate_temperature_s32(T_fine); //2508
    //double temperature = temperature_s32 / 100.; // 25.08
    uint32_t pressure_u32 = compensate_pressure_u32(calib, T_fine, adc_P); // 25767236
    //double pressure = pressure_u32 / 256.; //100653 (100653.27)
    //double altitude = calculate_altitude(pressure);


    if(T_fine != 2508) {
        return false;
    }


    if(temperature_s32 != 2508) {
        return false;
    }

    if(pressure_u32/100 != 25767236/100) {
        return false;
    }

    return true;
}


