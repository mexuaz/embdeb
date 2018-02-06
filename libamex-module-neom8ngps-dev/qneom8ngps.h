#ifndef QNEOM8NGPS_H
#define QNEOM8NGPS_H

#include <QObject>
#include <QtSerialPort/QSerialPort>

#include <map>
#include <string>

namespace amex {
    class QNeoM8nGPS : public QObject {
        Q_OBJECT

        QSerialPort m_serialPort;


    public:
        /*
            NMEA: National Marine Electronics Association
            NMEA 0183 v4.0


            $GPGSV,1,1,02,02,,,14,32,,,24*7B
            $GLGSV,1,1,03,,,,26,,,,25,,,,25*62
            $GNGLL,,,,,,V,N*7A
            $GNRMC,,V,,,,,,,,,,N*4D
            $GNVTG,,,,,,,,,N*2E
            $GNGGA,,,,,,0,00,99.99,,,,,,*56
            $GNGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*2E




        enum class nmea_sen_t{
            GPGSV,
            GLGSV,
            GNGLL,
            GNRMC,
            GNVTG,
            GNGGA,
            GNGSA,
        };

        typedef std::map<nmea_sen_t, std::string> nmea_t;

    private:

         Used to stored nmea starting field names in string
         format; such as $GPGSV

        nmea_t m_fieldNames;
       */


        typedef std::map<std::string, std::string> nmea_t;


    private:
        /*
         * Used to store nmea data read from GPS sensor
         */
        nmea_t m_nmea;


    public:
        explicit QNeoM8nGPS(const QString& serialPortName, int baudrate = QSerialPort::Baud115200, QObject *parent = Q_NULLPTR);
        ~QNeoM8nGPS();

        void get_info(nmea_t& nmea);


    signals:
        void dataReady();
        void finished(int code);

    private slots:
        void onReadyRead();


    public slots:
        void dump();
    }; // end of class QNeoM8nGPS
} // end of namespace amex



#endif // QNEOM8NGPS_H
