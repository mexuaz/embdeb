#ifndef BMP280_H
#define BMP280_H

#include "i2cbus.h"

namespace amex {
    class bmp280 {

        i2c m_i2c;

        struct MemoryMap {
            static const uint8_t DIG_T1         = 0x88; // [0..15]
            static const uint8_t DIG_T2         = 0x8A; // [0..15]
            static const uint8_t DIG_T3         = 0x8C; // [0..15]

            static const uint8_t DIG_P1         = 0x8E; // [0..15]
            static const uint8_t DIG_P2         = 0x90; // [0..15]
            static const uint8_t DIG_P3         = 0x92; // [0..15]
            static const uint8_t DIG_P4         = 0x94; // [0..15]
            static const uint8_t DIG_P5         = 0x96; // [0..15]
            static const uint8_t DIG_P6         = 0x98; // [0..15]
            static const uint8_t DIG_P7         = 0x9A; // [0..15]
            static const uint8_t DIG_P8         = 0x9C; // [0..15]
            static const uint8_t DIG_P9         = 0x9E; // [0..15]

            static const uint8_t CHIPID         = 0xD0;
            static const uint8_t VERSION        = 0xD1;
            static const uint8_t SOFTRESET      = 0xE0;

            static const uint8_t CAL26          = 0xE1;  // R calibration stored in 0xE1-0xF0

            static const uint8_t CONTROL        = 0xF4;
            static const uint8_t CONFIG         = 0xF5;
            static const uint8_t PRESSUREDATA   = 0xF7; // [0..19]
            static const uint8_t TEMPDATA       = 0xFA; // [0..19]
        };



        /*=========================================================================*/

        /*=========================================================================
            CALIBRATION DATA
            -----------------------------------------------------------------------*/
            struct bmp280_calib_data {
              uint16_t dig_T1;
              int16_t  dig_T2;
              int16_t  dig_T3;

              uint16_t dig_P1;
              int16_t  dig_P2;
              int16_t  dig_P3;
              int16_t  dig_P4;
              int16_t  dig_P5;
              int16_t  dig_P6;
              int16_t  dig_P7;
              int16_t  dig_P8;
              int16_t  dig_P9;

              uint8_t  dig_H1;
              int16_t  dig_H2;
              uint8_t  dig_H3;
              int16_t  dig_H4;
              int16_t  dig_H5;
              int8_t   dig_H6;
            } _bmp280_calib;
        /*=========================================================================*/


        static const uint8_t product_id = 0x58; // BMP 280 product id

        uint8_t id( void );

        /*
         * value 0xB6 is written to the register, the device
         *  is reset using the complete power-on-reset procedure
         */
        void set_soft_reset( void );


        void set_control( void );


        /*
         * Read coefficients from BMP280
         */
        void read_coefficients( bmp280_calib_data& calib );

        /*
         * Load sample coefficients from Datasheet for validate_datasheet_sample()
         */
        void sample_coefficients( bmp280_calib_data& calib );


        /*
         * Print coefficients
         */

        void print_coefficients( const bmp280_calib_data& calib );

        // read adc temperature, three registers in burst mode
        int32_t read_adc_temperature( void );

        // read adc Presure, three registers in burst mode
        int32_t read_adc_presure( void );

        // calculate T_fine based on adc_T
        int32_t calculate_temperature_fine(const bmp280_calib_data& calib, int32_t adc_T);

        // Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
        int32_t compensate_temperature_s32(int32_t temperature_fine);
        double compensate_temperature(int32_t temperature_fine);

        // Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
        // Output value of "24674867" represents 24674867/256 = 96386.2 Pa = 963.862 hPa
        uint32_t compensate_pressure_u32(const bmp280_calib_data& calib, int32_t temperature_fine, int32_t adc_P);
        double compensate_pressure(const bmp280_calib_data& calib, int32_t temperature_fine, int32_t adc_P); // in Si units for Pascal

        /*
         * validates calculation results based on datasheet values instead of real device values
         * this is for testing the calculations of this class
         */
        bool validate_datasheet_sample();

    public:
        explicit bmp280(unsigned int bus, unsigned int dev);
        ~bmp280();


        void read(double& temperature);

        void read(double&  temperature, double& pressure);

        double calculate_altitude(double pressure, double seaLevelhPa = 1013.25);

    }; // end of class bmp280
} // end of namespace amex


#endif // BMP280_H
