#ifndef ADIS16405_H
#define ADIS16405_H

#include "spibus.h"



namespace amex {
    class adis16405 {

        spi m_spi;

        struct MemoryMap {
            /* Measurement functions */
            static const uint8_t FLASH_CNT = 0x00; /* Flash memory write count */
            static const uint8_t SUPPLY_OUT = 0x02; /* Power supply measurement */
            static const uint8_t XGYRO_OUT = 0x04; /* X-axis gyroscope output */
            static const uint8_t YGYRO_OUT = 0x06; /* Y-axis gyroscope output */
            static const uint8_t ZGYRO_OUT = 0x08; /* Z-axis gyroscope output */
            static const uint8_t XACCL_OUT = 0x0A; /* X-axis accelerometer output */
            static const uint8_t YACCL_OUT = 0x0C; /* Y-axis accelerometer output */
            static const uint8_t ZACCL_OUT = 0x0E; /* Z-axis accelerometer output */
            static const uint8_t XMAGN_OUT = 0x10; /* X-axis magnetometer measurement */
            static const uint8_t YMAGN_OUT = 0x12; /* Y-axis magnetometer measurement */
            static const uint8_t ZMAGN_OUT = 0x14; /* Z-axis magnetometer measurement */
            static const uint8_t TEMP_OUT  = 0x16; /* Temperature output */
            static const uint8_t AUX_ADC   = 0x18; /* Auxiliary ADC measurement */

            /* Calibration parameters */
            static const uint8_t XGYRO_OFF = 0x1A; /* X-axis gyroscope bias offset factor */
            static const uint8_t YGYRO_OFF = 0x1C; /* Y-axis gyroscope bias offset factor */
            static const uint8_t ZGYRO_OFF = 0x1E; /* Z-axis gyroscope bias offset factor */
            static const uint8_t XACCL_OFF = 0x20; /* X-axis acceleration bias offset factor */
            static const uint8_t YACCL_OFF = 0x22; /* Y-axis acceleration bias offset factor */
            static const uint8_t ZACCL_OFF = 0x24; /* Z-axis acceleration bias offset factor */
            static const uint8_t XMAGN_HIF = 0x26; /* X-axis magnetometer, hard-iron factor */
            static const uint8_t YMAGN_HIF = 0x28; /* Y-axis magnetometer, hard-iron factor */
            static const uint8_t ZMAGN_HIF = 0x2A; /* Z-axis magnetometer, hard-iron factor */
            static const uint8_t XMAGN_SIF = 0x2C; /* X-axis magnetometer, soft-iron factor */
            static const uint8_t YMAGN_SIF = 0x2E; /* Y-axis magnetometer, soft-iron factor */
            static const uint8_t ZMAGN_SIF = 0x30; /* Z-axis magnetometer, soft-iron factor */

            static const uint8_t GPIO_CTRL = 0x32; /* Auxiliary digital input/output control */
            static const uint8_t MSC_CTRL  = 0x34; /* Miscellaneous control */
            static const uint8_t SMPL_PRD  = 0x36; /* Internal sample period (rate) control */
            static const uint8_t SENS_AVG  = 0x38; /* Dynamic range and digital filter control */
            static const uint8_t SLP_CNT   = 0x3A; /* Sleep mode control */
            static const uint8_t DIAG_STAT = 0x3C; /* System status */

            /* Alarm functions */
            static const uint8_t GLOB_CMD  = 0x3E; /* System command */
            static const uint8_t ALM_MAG1  = 0x40; /* Alarm 1 amplitude threshold */
            static const uint8_t ALM_MAG2  = 0x42; /* Alarm 2 amplitude threshold */
            static const uint8_t ALM_SMPL1 = 0x44; /* Alarm 1 sample size */
            static const uint8_t ALM_SMPL2 = 0x46; /* Alarm 2 sample size */
            static const uint8_t ALM_CTRL  = 0x48; /* Alarm control */
            static const uint8_t AUX_DAC   = 0x4A; /* Auxiliary DAC data */

            static const uint8_t PRODUCT_ID = 0x56; /* Product identifier */
        };

        static int32_t from_16bit_2s_complement(uint16_t value);

        static int32_t from_14bit_2s_complement(uint16_t value);

        static int32_t from_12bit_2s_complement(uint16_t value);

        static int32_t from_2s_complement(uint16_t value, uint8_t nbit);

        /*
         * This decodes the 14 bit raw data from the ADIS16405 sesor
         */
        static float decode_14bit(uint16_t value, float scale);

        /*
         * This decodes the 12 bit raw data from the ADIS16405 sesor
         */
        static float decode_12bit(uint16_t value, float scale);

        /*
         * Testing for device number, base 10 value (16405) is the model number (hex: 0x4015)
         */
        bool validate_device( void );

        /*
         * Power supply
         */
        float power_supply(void);

        /*
         * Gyroscope X
         */
        float gyroscope_x(void);

        /*
         * Gyroscope Y
         */
        float gyroscope_y(void);

        /*
         * Gyroscope Z
         */
        float gyroscope_z(void);

        /*
         * Accelerometer X
         */
        float accelerometer_x(void);

        /*
         * Accelerometer Y
         */
        float accelerometer_y(void);

        /*
         * Accelerometer Z
         */
        float accelerometer_z(void);

        /*
         * Magnetometer X
         */
        float magnetometer_x(void);

        /*
         * Magnetometer Y
         */
        float magnetometer_y(void);

        /*
         * Magnetometer Z
         */
        float magnetometer_z(void);


        /*
         * Temperature
         *
         * The scale factor is 0.14 degrees celcius per LSB, while 0x0000 = 25
         * degrees celcius.
         */
        float temperature(void);

        /*
         * AUX ADC
         */
        float adc(void);




    public:
        struct adisinfo_t {
            float power_supply;
            float gyroscope_x;
            float gyroscope_y;
            float gyroscope_z;
            float accelerometer_x;
            float accelerometer_y;
            float accelerometer_z;
            float magnetometer_x;
            float magnetometer_y;
            float magnetometer_z;
            float temperature;
            float aux_adc;
            adisinfo_t() {
                power_supply = 0.0f;
                gyroscope_x = 0.0f;
                gyroscope_y = 0.0f;
                gyroscope_z = 0.0f;
                accelerometer_x = 0.0f;
                accelerometer_y = 0.0f;
                accelerometer_z = 0.0f;
                magnetometer_x = 0.0f;
                magnetometer_y = 0.0f;
                magnetometer_z = 0.0f;
                temperature = 0.0f;
                aux_adc = 0.0f;
            }
        };



    private:


        /*
         * Software Reset
         *
         * Stops the sensor operation and runs the device through its start-up
         * sequence.
         */
        void reset_software( void );

        /*
         * Restoring sensors to factory calibration
         */
        void reset_factory( void );


    public:
        explicit adis16405(unsigned int bus, unsigned int dev);
        ~adis16405();

    public:
        /*
         * BURST MODE DATA COLLECTION
         */
        void data_burst( adisinfo_t& info );

    }; // end of class adis16405
} // end of namespce amex





#endif // ADIS16405_H
