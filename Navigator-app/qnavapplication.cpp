#include "qnavapplication.h"

#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include "navout.h"

QNavApplication::QNavApplication(int &argc, char **argv)
    : QCoreApplication(argc, argv)
    , m_socket(this)
    , m_timer_interval(1000)
    , m_console(false)
    , m_navout_calculations(false)
{

    QNavApplication::setApplicationName("Navigator");
    QNavApplication::setApplicationVersion("1.1.0");
    QNavApplication::setOrganizationName("M-Inc.");
    QNavApplication::setOrganizationDomain("qt.io");

    QObject::connect(&m_modules, SIGNAL(finished(int)), this, SLOT(onFinished(int)));
    QObject::connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimerTimeout()));
    m_timer.start(m_timer_interval);

}

void QNavApplication::set_license(quint64 key) {
    try {
        m_modules.set_license(key);
    } catch (std::exception& e) {
        qCritical() << e.what();
        quit();
    }

}

bool QNavApplication::is_licensed() const
{
    return m_modules.is_licensed();
}

void QNavApplication::set_timer_interval(int msecs)
{
    m_timer_interval = msecs;
    m_timer.start(m_timer_interval);
}

void QNavApplication::set_port_host(const QString &host, quint16 port)
{
    m_host = host;
    m_port = port;
}

#define LOGHeader "\
localtime,\
temperature,pressure,altitude,\
power_supply,gyroscope_x,gyroscope_y,gyroscope_z,\
accelerometer_x,accelerometer_y,accelerometer_z,\
magnetometer_x,magnetometer_y,magnetometer_z,\
temperature,aux_adc,\
GPGSV,GLGSV,GNGLL,GNRMC,GNVTG,GNGGA,GNGSA\
lat,lng,alt,nav_phi,nav_Theta,nav_psi,mag_psi,gps_psi,nav_north_vel,nav_east_vel,nav_down_vel,gps_quality,gps_ready,\r\n"


void QNavApplication::set_logfile(const QString &logfilename)
{
    m_logfile.setFileName(logfilename);
    if(!m_logfile.open(QIODevice::WriteOnly)) {
        qCritical() << m_logfile.errorString();
    }

    m_logfile.write(LOGHeader);
}

void QNavApplication::set_console(bool console)
{
    m_console = console;
}

void QNavApplication::set_navigation_calculations(bool status)
{
    try {
        m_modules.init_gpio();
    } catch(std::exception& e) {
        qCritical() << e.what();
    }

    m_navout_calculations = status;
}

void QNavApplication::set_input_settings(const inputSettings_t &inputSettings)
{
    m_inputSettings = inputSettings;
}

void QNavApplication::start_gps(const QString &serialPortName, int baudrate) {
    try {
        m_modules.connect_neom8ngps(serialPortName, baudrate, this);
    } catch (std::exception& e) {
        qCritical() << e.what();
        quit();
    }
}

void QNavApplication::start_barometer(unsigned int bus, unsigned int dev) {
    try {
        m_modules.connect_bmp280(bus, dev);
    } catch (std::exception& e) {
        qCritical() << e.what();
        quit();
    }
}

void QNavApplication::start_imu(unsigned int bus, unsigned int dev) {
    try {
        m_modules.connect_adis16405(bus, dev);
    } catch (std::exception& e) {
        qCritical() << e.what();
        quit();
    }
}

void QNavApplication::start_serialout(const QString &serialPortName, int baudrate)
{
    try {
        m_modules.connect_serialout(serialPortName, baudrate, this);
    } catch (std::exception& e) {
        qCritical() << e.what();
        quit();
    }
}

bool QNavApplication::set_service(bool enable)
{
    return m_modules.set_service(enable);
}

bool QNavApplication::run_qt_compile(const QString& workingdir)
{
    return m_modules.run_qt_compile(workingdir);
}

bool QNavApplication::run_make_install(const QString& targetdir)
{
    return m_modules.run_make_install(targetdir);
}

void QNavApplication::onFinished(int code)
{
    qCritical() << "Application is signaled to quit with code: " << code;
    QTimer::singleShot(0, this, SLOT(quit())); // postpone the quit unitl the event loop get started app.exec()
}

void QNavApplication::onTimerTimeout() {


    QJsonObject jObj_dataPacket;


    QString strTime = QDateTime::currentDateTime().toString(Qt::ISODate).replace('T', ' ');

    jObj_dataPacket["localtime"] = strTime;

    QByteArray logline(strTime.toLatin1()); // start log file line with local time field



    /*
     * Barometer Data
     */
    bmp280_t bmp;
    memset(&bmp, 0, sizeof(bmp280_t));
    if(m_modules.is_connected_bmp280()) {
        m_modules.read_bmp280(bmp);


        if(!m_host.isEmpty()) {
            QJsonObject jObj_bmpd;
            jObj_bmpd["ProductName"] = "BMP";
            jObj_bmpd["ProductID"] = "280";
            jObj_dataPacket["Barometer-Device"] = jObj_bmpd;


            QJsonObject jObj_bmp;
            jObj_bmp["temperature"] = bmp.temperature;
            jObj_bmp["pressure"] = bmp.pressure;
            jObj_bmp["altitude"] = bmp.altitude;
            jObj_dataPacket["Barometer"] = jObj_bmp;
        }


        if(m_logfile.isOpen()) {
            if(!logline.isEmpty()) {
                logline.append(',');
            }
            logline.append(QByteArray::number(bmp.temperature)+",");
            logline.append(QByteArray::number(bmp.pressure)+",");
            logline.append(QByteArray::number(bmp.altitude));
        }

        if(m_console) {
            qDebug("Temperature: %+8.2f, Pressure: %+8.2f, Altitude: %+8.2f",
                   bmp.temperature,
                   bmp.pressure,
                   bmp.altitude);
        }
    } // end of barometer is enabled




    /*
     * IMU Data
     */
    adisInfo_t adis;
    memset(&adis, 0, sizeof(adisInfo_t));
    if(m_modules.is_connected_adis16405()) {  
        m_modules.read_adis16405(adis);

        if(!m_host.isEmpty()) {
            QJsonObject jObj_imud;
            jObj_imud["ProductName"] = "ADIS";
            jObj_imud["ProductID"] = "16405";
            jObj_dataPacket["IMU-Device"] = jObj_imud;

            QJsonObject jObj_imu;
            jObj_imu["power_supply"] = static_cast<double>(adis.power_supply);
            jObj_imu["gyroscope_x"] = static_cast<double>(adis.gyroscope_x);
            jObj_imu["gyroscope_y"] = static_cast<double>(adis.gyroscope_y);
            jObj_imu["gyroscope_z"] = static_cast<double>(adis.gyroscope_z);
            jObj_imu["accelerometer_x"] = static_cast<double>(adis.accelerometer_x);
            jObj_imu["accelerometer_y"] = static_cast<double>(adis.accelerometer_y);
            jObj_imu["accelerometer_z"] = static_cast<double>(adis.accelerometer_z);
            jObj_imu["magnetometer_x"] = static_cast<double>(adis.magnetometer_x);
            jObj_imu["magnetometer_y"] = static_cast<double>(adis.magnetometer_y);
            jObj_imu["magnetometer_z"] = static_cast<double>(adis.magnetometer_z);
            jObj_imu["temperature"] = static_cast<double>(adis.temperature);
            jObj_imu["aux_adc"] = static_cast<double>(adis.aux_adc);
            jObj_dataPacket["IMU"] = jObj_imu;
        }

        if(m_logfile.isOpen()) {
            if(!logline.isEmpty()) {
                logline.append(',');
            }
            logline.append(QByteArray::number(static_cast<double>(adis.power_supply))+",");
            logline.append(QByteArray::number(static_cast<double>(adis.gyroscope_x))+",");
            logline.append(QByteArray::number(static_cast<double>(adis.gyroscope_y))+",");
            logline.append(QByteArray::number(static_cast<double>(adis.gyroscope_z))+",");
            logline.append(QByteArray::number(static_cast<double>(adis.accelerometer_x))+",");
            logline.append(QByteArray::number(static_cast<double>(adis.accelerometer_y))+",");
            logline.append(QByteArray::number(static_cast<double>(adis.accelerometer_z))+",");
            logline.append(QByteArray::number(static_cast<double>(adis.magnetometer_x))+",");
            logline.append(QByteArray::number(static_cast<double>(adis.magnetometer_y))+",");
            logline.append(QByteArray::number(static_cast<double>(adis.magnetometer_z))+",");
            logline.append(QByteArray::number(static_cast<double>(adis.temperature))+",");
            logline.append(QByteArray::number(static_cast<double>(adis.aux_adc)));
        }

        if(m_console) {
            qDebug("%+*.2f, %+*.2f, %+*.2f, %+*.2f, %+*.2f, %+*.2f, %+*.2f, %+*.2f, %+*.2f, %+*.2f, %+*.2f, %+*.2f",
                   4, static_cast<double>(adis.power_supply),
                   5, static_cast<double>(adis.gyroscope_x),
                   5, static_cast<double>(adis.gyroscope_y),
                   5, static_cast<double>(adis.gyroscope_z),
                   5, static_cast<double>(adis.accelerometer_x),
                   5, static_cast<double>(adis.accelerometer_y),
                   5, static_cast<double>(adis.accelerometer_z),
                   5, static_cast<double>(adis.magnetometer_x),
                   5, static_cast<double>(adis.magnetometer_y),
                   5, static_cast<double>(adis.magnetometer_z),
                   4, static_cast<double>(adis.temperature),
                   4, static_cast<double>(adis.aux_adc));
        }
    } // end of imu is enabled










    /*
     * GPS/GNS Data
     */


    nmea_t nmea;
    if( m_modules.is_connected_neom8ngps()) {
        m_modules.read_neom8ngps(nmea);

        if(!m_host.isEmpty()) {
            QJsonObject jObj_gpsd;
            jObj_gpsd["ProductName"] = "EOM8N";
            jObj_gpsd["ProductID"] = "";
            jObj_dataPacket["GPS-Device"] = jObj_gpsd;

            QJsonObject jObj_gps;
            for(const auto& a : nmea) {
                jObj_gps[a.first.c_str()] = QString(a.second.c_str());
            }
            jObj_dataPacket["GNS"] = jObj_gps;
        }


        if(m_logfile.isOpen()) {
            if(!logline.isEmpty()) {
                logline.append(',');
            }
            /*
             * log file fields count should be fixed to produce fixed size csv and match the columns number
             */
            logline.append("\""+QByteArray(nmea["GPGSV"].c_str())+"\",");
            logline.append("\""+QByteArray(nmea["GLGSV"].c_str())+"\",");
            logline.append("\""+QByteArray(nmea["GNGLL"].c_str())+"\",");
            logline.append("\""+QByteArray(nmea["GNRMC"].c_str())+"\",");
            logline.append("\""+QByteArray(nmea["GNVTG"].c_str())+"\",");
            logline.append("\""+QByteArray(nmea["GNGGA"].c_str())+"\",");
            logline.append("\""+QByteArray(nmea["GNGSA"].c_str())+"\",");
        }

        if(m_console) {

            for(const auto& a : nmea) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
                qDebug().noquote().nospace() << a.first.c_str() << ": " << a.second.c_str();
#else
                qDebug() << qPrintable(std::string( a.first + ": " + a.second ).c_str());
#endif
            }

        }
    } // end of gps is enabled



    /*
     * Nav output data
     */

    if(m_navout_calculations) {

        int led;
        usedGNS_t usedGNS;
        navMode_t navMode;
        AHRS_out_t ahrsOut;
        navOut_t navOut;

        qint64 elapsed = this->m_elspseTimer.elapsed();
        m_elspseTimer.start();
        Navout::nav_calc(
                    static_cast<unsigned int>(elapsed),
                    m_inputSettings,
                    nmea,
                    adis,
                    bmp,
                    led,
                    usedGNS,
                    navMode,
                    ahrsOut,
                    navOut);

        m_modules.set_gpio(led);

        static int n = 0;

        QJsonObject jObj_etc;
        jObj_etc["dt"] = elapsed;
        jObj_etc["times"] = ++n;
        jObj_dataPacket["etc"] = jObj_etc;


        if(!m_host.isEmpty()) {

            QJsonObject jObj_navout;

            jObj_navout["lat"] = static_cast<double>(navOut.Lat);
            jObj_navout["lng"] = static_cast<double>(navOut.Long);
            jObj_navout["alt"] = static_cast<double>(navOut.Alt);

            jObj_navout["north_vel"] = static_cast<double>(navOut.Nav_North_Vel);
            jObj_navout["east_vel"] = static_cast<double>(navOut.Nav_East_Vel);
            jObj_navout["down_vel"] = static_cast<double>(navOut.Nav_Down_Vel);

            jObj_navout["nav_phi"] = static_cast<double>(navOut.Nav_Phi);
            jObj_navout["nav_theta"] = static_cast<double>(navOut.Nav_Theta);
            jObj_navout["nav_psi"] = static_cast<double>(navOut.Nav_Psi);

            jObj_navout["mag_psi"] = static_cast<double>(navOut.Mag_Psi);
            jObj_navout["gns_psi"] = static_cast<double>(navOut.GNS_Psi);
            jObj_navout["gns_quality"] = static_cast<int>(navOut.GNS_Quality);



            jObj_dataPacket["NavOut"] = jObj_navout;


            /*
             * AHRS
             */

            QJsonObject jObj_ahrsout;

            jObj_ahrsout["lat"] = static_cast<double>(ahrsOut.Lat);
            jObj_ahrsout["lng"] = static_cast<double>(ahrsOut.Long);
            jObj_ahrsout["alt"] = static_cast<double>(ahrsOut.Alt);
            jObj_ahrsout["north_vel"] = static_cast<double>(ahrsOut.North_Vel);
            jObj_ahrsout["east_vel"] = static_cast<double>(ahrsOut.East_Vel);
            jObj_ahrsout["down_vel"] = static_cast<double>(ahrsOut.Down_Vel);
            jObj_ahrsout["phi"] = static_cast<double>(ahrsOut.Phi);
            jObj_ahrsout["theta"] = static_cast<double>(ahrsOut.Theta);
            jObj_ahrsout["psi"] = static_cast<double>(ahrsOut.Psi);

            jObj_dataPacket["AHRS_Out"] = jObj_ahrsout;



            /*
             * Nav Mode
             */

            QJsonObject jObj_navmode;

            jObj_navmode["mode"] = static_cast<int>(navMode.Mode);
            jObj_navmode["status"] = static_cast<int>(navMode.Status);
            jObj_navmode["align_time"] = static_cast<int>(navMode.AlignTime);
            jObj_navmode["vehicle"] = static_cast<int>(navMode.vehicle);

            jObj_dataPacket["NavMode"] = jObj_navmode;


            /*
             * Used GNS
             */

            QJsonObject jObj_usedGNS;

            jObj_usedGNS["gns_lat"] = static_cast<double>(usedGNS.Lat);
            jObj_usedGNS["gns_long"] = static_cast<double>(usedGNS.Long);
            jObj_usedGNS["gns_alt"] = static_cast<double>(usedGNS.Alt);
            jObj_usedGNS["gns_north_vel"] = static_cast<double>(usedGNS.North_Vel);
            jObj_usedGNS["gns_east_vel"] = static_cast<double>(usedGNS.East_Vel);
            jObj_usedGNS["gns_down_vel"] = static_cast<double>(usedGNS.Down_Vel);
            jObj_usedGNS["gns_lat_long_hen"] = static_cast<double>(usedGNS.LatLongHen);
            jObj_usedGNS["gns_sats"] = static_cast<double>(usedGNS.Sats);
            jObj_usedGNS["gns_quality"] = static_cast<double>(usedGNS.Quality);
            jObj_usedGNS["gns_time"] = static_cast<double>(usedGNS.Time);
            jObj_usedGNS["gns_date"] = static_cast<double>(usedGNS.Date);
            jObj_usedGNS["gns_type"] = static_cast<double>(usedGNS.GNS_Type);

            jObj_dataPacket["Used_GNS"] = jObj_usedGNS;



        }


        if(m_modules.is_connected_serialout()) {
            char endline[] = {"\r\n"};
            m_modules.write_serialout(reinterpret_cast<void*>(endline), sizeof(endline));
            //m_modules.write_serialout(reinterpret_cast<void*>(endline), sizeof(endline));
            m_modules.write_serialout(reinterpret_cast<void*>(&navOut), sizeof(navOut));
            m_modules.write_serialout(reinterpret_cast<void*>(&ahrsOut), sizeof(ahrsOut));
            m_modules.write_serialout(reinterpret_cast<void*>(&usedGNS), sizeof(usedGNS));
            //m_modules.write_serialout(reinterpret_cast<void*>(endline), sizeof(endline));
            //m_modules.write_serialout(reinterpret_cast<void*>(endline), sizeof(endline));
        }


        if(m_logfile.isOpen()) {
            if(!logline.isEmpty()) {
                logline.append(',');
            }
            logline.append(QByteArray::number(static_cast<double>(navOut.Lat))+",");
            logline.append(QByteArray::number(static_cast<double>(navOut.Long))+",");
            logline.append(QByteArray::number(static_cast<double>(navOut.Alt))+",");

            logline.append(QByteArray::number(static_cast<double>(navOut.Nav_North_Vel))+",");
            logline.append(QByteArray::number(static_cast<double>(navOut.Nav_East_Vel))+",");
            logline.append(QByteArray::number(static_cast<double>(navOut.Nav_Down_Vel))+",");

            logline.append(QByteArray::number(static_cast<double>(navOut.Nav_Phi))+",");
            logline.append(QByteArray::number(static_cast<double>(navOut.Nav_Theta))+",");
            logline.append(QByteArray::number(static_cast<double>(navOut.Nav_Psi))+",");

            logline.append(QByteArray::number(static_cast<double>(navOut.Mag_Psi))+",");
            logline.append(QByteArray::number(static_cast<double>(navOut.GNS_Psi))+",");
            logline.append(QByteArray::number(static_cast<double>(navOut.GNS_Quality))+",");



            logline.append(QByteArray::number(static_cast<double>(ahrsOut.Lat))+",");
            logline.append(QByteArray::number(static_cast<double>(ahrsOut.Long))+",");
            logline.append(QByteArray::number(static_cast<double>(ahrsOut.Alt))+",");

            logline.append(QByteArray::number(static_cast<double>(ahrsOut.North_Vel))+",");
            logline.append(QByteArray::number(static_cast<double>(ahrsOut.East_Vel))+",");
            logline.append(QByteArray::number(static_cast<double>(ahrsOut.Down_Vel))+",");

            logline.append(QByteArray::number(static_cast<double>(ahrsOut.Phi))+",");
            logline.append(QByteArray::number(static_cast<double>(ahrsOut.Theta))+",");
            logline.append(QByteArray::number(static_cast<double>(ahrsOut.Psi))+",");


        }

        if(m_console) {
            qDebug("%+*.2f, %+*.2f, %+*.2f, %+*.2f, %+*.2f, %+*.2f, %+*.2f, %+*.2f, %+*.2f, %+*.2f, %+*.2f, %u",
                   4, static_cast<double>(navOut.Lat),
                   5, static_cast<double>(navOut.Long),
                   5, static_cast<double>(navOut.Alt),

                   5, static_cast<double>(navOut.Nav_North_Vel),
                   5, static_cast<double>(navOut.Nav_East_Vel),
                   5, static_cast<double>(navOut.Nav_Down_Vel),

                   5, static_cast<double>(navOut.Nav_Phi),
                   5, static_cast<double>(navOut.Nav_Theta),
                   5, static_cast<double>(navOut.Nav_Psi),

                   5, static_cast<double>(navOut.Nav_Psi),
                   4, static_cast<double>(navOut.GNS_Psi),
                   navOut.GNS_Quality);


            qDebug("%+*.2f, %+*.2f, %+*.2f, %+*.2f, %+*.2f, %+*.2f, %+*.2f, %+*.2f, %+*.2f",
                   4, static_cast<double>(ahrsOut.Lat),
                   5, static_cast<double>(ahrsOut.Long),
                   5, static_cast<double>(ahrsOut.Alt),

                   5, static_cast<double>(ahrsOut.North_Vel),
                   5, static_cast<double>(ahrsOut.East_Vel),
                   5, static_cast<double>(ahrsOut.Down_Vel),

                   5, static_cast<double>(ahrsOut.Phi),
                   5, static_cast<double>(ahrsOut.Theta),
                   5, static_cast<double>(ahrsOut.Psi));
        }




    } // end of calculations is enabled





    jObj_dataPacket["IsUsed"] = false;

    QJsonObject jObj_root;
    jObj_root["DataPacket"] = jObj_dataPacket;

    QJsonDocument data(jObj_root);
    if(!m_host.isEmpty()) {
        m_socket.writeDatagram(data.toJson(QJsonDocument::Compact), QHostAddress(m_host), m_port);
    }

    if(m_logfile.isOpen()) {
        m_logfile.write(logline);
        m_logfile.write("\r\n");
        m_logfile.flush();
    }




}
