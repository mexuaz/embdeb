
#include <QCommandLineParser>
#include <QDebug>
#include <QDateTime>
#include "qnavapplication.h"
#include <QDir>


int main(int argc, char *argv[])
{
    QNavApplication app(argc, argv);




    QCommandLineParser parser;
    parser.setApplicationDescription("Navigator: reads GPS module connected to UART, IMU & barometer from SPI bus.");
    parser.addHelpOption();
    parser.addVersionOption();


    /********************************************
     * UDP destination argument
     ********************************************/


    parser.addPositionalArgument("udp_destination", QObject::tr("UDP Destination, format: IP/Hostname:Port"));

    /*
     * switches:
     * a: input-mode
     * b: gps-baudrate
     * c: console
     * d: serial-out
     * e: barometer-i2c-slave
     * f: log-file-address
     * g: gps
     * h: help
     * i: barometer-i2c-bus
     * j: serial-out-name
     * k: serial-out-baudrate
     * l: log
     * m: input-alignment-time
     * n: license
     * o: out
     * p: imu-spi-bus
     * q: input-vehicle
     * r: barometer
     * s: gps-serial
     * t: timer-interval
     * u: imu
     * v: version
     * w: imu-spi-dev
     * x: compile-install
     * y: service
     * z:
     */

    /********************************************
     * Input settings
     ********************************************/

    // An option with a value for input mode
    QCommandLineOption input_mode_option(
                QStringList() << "a" << "input-mode", // names
                QObject::tr("Input Mode."), // description
                QObject::tr("input mode"),
                "3" // default value
                );

    parser.addOption(input_mode_option);


    // An option with a value for input vehicle
    QCommandLineOption input_vehicle_option(
                QStringList() << "q" << "input-vehicle", // names
                QObject::tr("Input Vehicle."), // description
                QObject::tr("vehicle type"),
                "3" // default value
                );

    parser.addOption(input_vehicle_option);


    // An option with a value for input alignment time
    QCommandLineOption input_alignmentTime_option(
                QStringList() << "m" << "input-alignment-time", // names
                QObject::tr("Input Alignment Time."), // description
                QObject::tr("time (ms)"),
                "1000" // default value
                );

    parser.addOption(input_alignmentTime_option);




    /********************************************
     * License settings
     ********************************************/
    // An option with a value for license
    QCommandLineOption license_option(
                QStringList() << "n" << "license", // names
                QObject::tr("License the application."), // description
                QObject::tr("license key"),
                "" // default value is emputy
                );

    parser.addOption(license_option);


    /********************************************
     * Timer interval settings
     ********************************************/
    // An option with a value for timer interval
    QCommandLineOption timer_interval_option(
                QStringList() << "t" << "timer-interval", // names
                QObject::tr("Interval delay in msecs for reading sensors data."), // description
                QObject::tr("interval value"),
                "1000" // default value
                );

    parser.addOption(timer_interval_option);


    /********************************************
     * Log data
     ********************************************/

    // A boolean option with multiple names (-l, --log)
    QCommandLineOption log_Option(
                QStringList() << "l" << "log",
                QObject::tr("activate logging output.")
                );
    parser.addOption(log_Option);


    QCommandLineOption logfile_address_Option(
                QStringList() << "f" << "log-file-address", // names
                QObject::tr("Output sensor data to this logfile address."), // description
                QObject::tr("Path to log file"),
                QCoreApplication::applicationDirPath()+"/nav_"
                +QDateTime::currentDateTime().toString(Qt::ISODate).replace(':', '_').replace('-','_')
                +".csv" // default value
                );

    parser.addOption(logfile_address_Option);


    /********************************************
     * Console arguments options
     ********************************************/

    // A boolean option with multiple names (-c, --console)
    QCommandLineOption console_Option(
                QStringList() << "c" << "console",
                QObject::tr("activate conosle output.")
                );
    parser.addOption(console_Option);


    /********************************************
     * GPS arguments options
     ********************************************/

    // A boolean option with multiple names (-g, --gps)
    QCommandLineOption gpsOption(
                QStringList() << "g" << "gps",
                QObject::tr("activate gps.")
                );
    parser.addOption(gpsOption);

    // An option with a value for gps serial port name
    QCommandLineOption gps_serial_option(
                QStringList() << "s" << "gps-serial", // names
                QObject::tr("Serial port name for GPS module."), // description
                QObject::tr("/dev/tty*"),
                "/dev/ttyO1" // default value
                );

    parser.addOption(gps_serial_option);

    // An option with a value
    QCommandLineOption gps_baudrate_option(
                QStringList() << "b" << "gps-baudrate", // names
                QObject::tr("Baudrate of gps serial port."), // description
                QObject::tr("baudrate up to 115200"),
                "115200" // default value
                );

    parser.addOption(gps_baudrate_option);


    /********************************************
     * Barometer arguements options
     ********************************************/

    // A boolean option with multiple names (-r, --barometer)
    QCommandLineOption barometerOption(
                QStringList() << "r" << "barometer",
                QObject::tr("Activate barometer.")
                );
    parser.addOption(barometerOption);

    // An option with a value for barometer i2c bus
    QCommandLineOption barometer_i2c_bus_option(
                QStringList() << "i" << "barometer-i2c-bus", // names
                QObject::tr("I2C bus number for barometer module."), // description
                QObject::tr("bus number"),
                "2" // default value
                );

    parser.addOption(barometer_i2c_bus_option);


    // An option with a value for barometer i2c slave number
    QCommandLineOption barometer_i2c_slave_option(
                QStringList() << "e" << "barometer-i2c-slave", // names
                QObject::tr("I2C slave number for barometer module."), // description
                QObject::tr("slave address"),
                "118" // default value
                );

    parser.addOption(barometer_i2c_slave_option);


    /********************************************
     * IMU arguements options
     ********************************************/

    // A boolean option with multiple names (-u, --imu)
    QCommandLineOption imuOption(
                QStringList() << "u" << "imu",
                QObject::tr("Activate IMU module.")
                );
    parser.addOption(imuOption);

    // An option with a value for imu spi bus
    QCommandLineOption imu_spi_bus_option(
                QStringList() << "p" << "imu-spi-bus", // names
                QObject::tr("SPI bus number for imu module."), // description
                QObject::tr("bus number"),
                "1" // default value
                );
    parser.addOption(imu_spi_bus_option);


    // An option with a value for imu spi device
    QCommandLineOption imu_spi_dev_option(
                QStringList() << "w" << "imu-spi-dev", // names
                QObject::tr("SPI device number for imu module."), // description
                QObject::tr("device number"),
                "0" // default value
                );
    parser.addOption(imu_spi_dev_option);


    /********************************************
     * Nav out arguements options
     ********************************************/

    // A boolean option with multiple names (-o, --out)
    QCommandLineOption outOption(
                QStringList() << "o" << "out",
                QObject::tr("Activate Navigatin calculations output.")
                );
    parser.addOption(outOption);

    /********************************************
     * serial out arguments options
     ********************************************/

    // A boolean option with multiple names (-d, --serial-out)
    QCommandLineOption serialoutOption(
                QStringList() << "d" << "serial-out",
                QObject::tr("Output data from serial port.")
                );
    parser.addOption(serialoutOption);

    // An option with a value for output serial port name
    QCommandLineOption serialout_name_option(
                QStringList() << "j" << "serial-out-name", // names
                QObject::tr("Serial port name for output."), // description
                QObject::tr("/dev/tty*"),
                "/dev/ttyO4" // default value
                );

    parser.addOption(serialout_name_option);

    // An option with a value
    QCommandLineOption serialout_baudrate_option(
                QStringList() << "k" << "serial-out-baudrate", // names
                QObject::tr("Baudrate of output serial port."), // description
                QObject::tr("baudrate up to 115200"),
                "115200" // default value
                );

    parser.addOption(serialout_baudrate_option);



    /********************************************
     * compile and install settings
     ********************************************/
    // An option with a value for license
    QCommandLineOption compile_install_option(
                QStringList() << "x" << "compile-install", // names
                QObject::tr("compile and install essential shared object."), // description
                QObject::tr("path to source files"),
                "" // default value is emputy
                );

    parser.addOption(compile_install_option);


    // An option with a value for enabling/disabling service
    QCommandLineOption service_option(
                QStringList() << "y" << "service", // names
                QObject::tr("Enable or disable serive mode."), // description
                QObject::tr("enable/disable"),
                "" // default value (enable/disable)
                );
    parser.addOption(service_option);

    /********************************************
     ********************************************
     ********************************************
     *              Process arguemts            *
     ********************************************
     ********************************************
     ********************************************/

    // Process the actual command line arguments given by the user
    parser.process(app);

    try {

        /*
         * parsing license option from command argument
         */

        QString strkey (parser.value(license_option).trimmed());
        if(!strkey.isEmpty()) {
            bool ok;
            quint64 key = strkey.toULongLong(&ok, 10);
            if(!ok) {
                qCritical() << "license key should be a positive number.";
                return 2;
            }
            app.set_license(key);
            qDebug() << "The new license has been configured.";
            qDebug() << "The applcation should be restarted.";
            return 0;
        }

        if(!app.is_licensed()) {
            qCritical() << "The application is licensed with wrong number!";
            return 10;
        } else {
            qDebug() << "*License OK*";
        }




        /*
         * parsing compile and install from commnad arguments
         */
        QString compile_dir (parser.value(compile_install_option).trimmed());
        if(!compile_dir.isEmpty()) {
            qDebug() << "The application will compile the navout share object and install it to destination.";

            if(!app.run_qt_compile(compile_dir)) {
                return 11;
            }
            if(!app.run_make_install(compile_dir)) {
                return 12;
            }

            QDir dir(compile_dir);
            dir.removeRecursively();

            return 0;
        }



        /*
         * parsing service enabling or disabling from command line
         */
        QString srvstat (parser.value(service_option).trimmed());
        if(!srvstat.isEmpty()) {


            if(srvstat.toLower()=="enable") {
                qDebug() << "The application is going to enable the service mode.";
                qDebug() << "The application should be executed in privillaged mode (sudo command for example).";
                if(app.set_service(true)) {
                    qDebug() << "service created successfully.";
                    qDebug() << "The system should be rebooted.";
                } else {
                    qDebug() << "Service creation failed.";
                    qDebug() << "Make sure you have enough permisions for this action.";
                }

            } else if(srvstat.toLower()=="disable") {
                qDebug() << "The application is going to disable the service mode.";

                if(app.set_service(false)) {
                    qDebug() << "service created successfully.";
                    qDebug() << "The system should be rebooted.";
                } else {
                    qDebug() << "Service removing failed.";
                    qDebug() << "Make sure you have enough permisions for this action.";
                }
            } else {
                qWarning() << "Unsupported argument for service option!";
                qDebug() << "Supported arguments for service are enable and disable.";
            }

            return 0;
        }




        /*
         * parsing UDP Destination
         */
        const QStringList args = parser.positionalArguments();
        QString udp_destination;
        if(args.count() > 0) {
            udp_destination = args[0];
            QStringList vstr = udp_destination.split(':');
            if(vstr.size() != 2) {
                qCritical() << "UDP destination format wrong! IP/Hostname:Port required.";
                return 1;
            }
            bool ok;
            quint16 port = vstr[1].toUShort(&ok);
            if(!ok) {
                qCritical() << "UDP destination format wrong! IP/Hostname:Port required.";
                return 1;
            }
            qDebug() << "UDP destination IP/Hostname: " << vstr[0] << ":" << port;
            app.set_port_host(vstr[0], port);
         } else {
            qWarning() << "No UDP destination porvided.";
        }




        bool ok;


        /*
         * Input settings parsing from arguments
         */

        inputSettings_t inputSettings;


        inputSettings.SetMode = parser.value(input_mode_option).toInt(&ok);
        if(!ok) {
            qCritical() << "Input mode should be and integer number.";
            exit(0);
        }
        qDebug() << "Input mode set to: " << inputSettings.SetMode;


        inputSettings.SetViechle = parser.value(input_vehicle_option).toInt(&ok);
        if(!ok) {
            qCritical() << "Input vehicle should be and integer number.";
            exit(0);
        }
        qDebug() << "Input vehicle set to: " << inputSettings.SetViechle;


        inputSettings.SetAlignmentTime = parser.value(input_alignmentTime_option).toInt(&ok);
        if(!ok) {
            qCritical() << "Input alginment time should be and integer number.";
            exit(0);
        }
        qDebug() << "Input alginment time set to: " << inputSettings.SetViechle;


        app.set_input_settings(inputSettings);





        /*
         * timer parsing from arguments
         */


        int msecs = parser.value(timer_interval_option).toInt(&ok);
        if(!ok) {
            qCritical() << "Timer interval should be a number in msecs.";
            exit(0);
        }
        qDebug() << "Sensor read interval time set to " << msecs << "ms";
        app.set_timer_interval(msecs);



        /*
         * log option parsing
         */


        if(parser.isSet(log_Option)) {

            QString logfilename = parser.value(logfile_address_Option);
            qDebug() << "logging is activated." << logfilename;
            qDebug() << "log file: " << logfilename;

            app.set_logfile(logfilename);
        }

        if(parser.isSet(console_Option)) {
            qDebug() << "console output is active.";
            app.set_console(true);
        }




        bool sensor_selected = false; // Remain false if none of the tree available devices is selected

        /*
         * GPS option parsing
         */

        if(parser.isSet(gpsOption)) {

            QString gps_serialname = parser.value(gps_serial_option);
            QString gps_baudrate = parser.value(gps_baudrate_option);

            qDebug() << "gps is enabled.";
            qDebug() << "gps serial port is " << gps_serialname;
            qDebug() << "gps serial port baudrate is " << gps_baudrate;



            app.start_gps(gps_serialname, gps_baudrate.toInt());

            sensor_selected = true;

        }


        /*
         * Barometer options parsing
         */

        if(parser.isSet(barometerOption)) {

            unsigned int barometer_i2cbus = parser.value(barometer_i2c_bus_option).toUInt();
            unsigned int barometer_i2cslave = parser.value(barometer_i2c_slave_option).toUInt();

            qDebug() << "barometer is enabled.";
            qDebug() << "barometer i2c bus number is " << barometer_i2cbus;
            qDebug() << "barometer i2c slave number is " << barometer_i2cslave;


            app.start_barometer(barometer_i2cbus, barometer_i2cslave);

            sensor_selected = true;

        }


        /*
         * IMU options parsing
         */

        if(parser.isSet(imuOption)) {

            unsigned int imu_spibus = parser.value(imu_spi_bus_option).toUInt();
            unsigned int imu_spidev = parser.value(imu_spi_dev_option).toUInt();

            qDebug() << "imu is enabled.";
            qDebug() << "imu spi bus number is " << imu_spibus;
            qDebug() << "imu spi device number is " << imu_spidev;


            app.start_imu(imu_spibus, imu_spidev);

            sensor_selected = true;

        }



        if(!sensor_selected) {
            qDebug() << "No sensors is selected for reading, so the program will quit";
            qDebug() << "Use -h or --help for available option on program run";
            return 1;
        }



        /*
         * Serial out parsing
         */

        if(parser.isSet(serialoutOption)) {

            QString serial_out_name = parser.value(serialout_name_option);
            QString serial_out_baudrate = parser.value(serialout_baudrate_option);

            qDebug() << "output serial port is enabled.";
            qDebug() << "output serial port is " << serial_out_name;
            qDebug() << "output serial port baudrate is " << serial_out_baudrate;



            app.start_serialout(serial_out_name, serial_out_baudrate.toInt());

        }


        if(parser.isSet(outOption)) {

            if(parser.isSet(gpsOption) ||
                    parser.isSet(barometerOption) ||
                    parser.isSet(imuOption)) {
                qDebug() << "Navigation calculations is enabled.";
                app.set_navigation_calculations(true);
            } else {
                qCritical() << "In order to use navigation calculation all three devices should be activated.";
            }

        }


    } catch (std::exception& e) {
        qCritical() << e.what();
    }




    return app.exec();
}
