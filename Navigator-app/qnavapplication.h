#ifndef QNAVAPPLICATION_H
#define QNAVAPPLICATION_H

#include <QCoreApplication>
#include <QTimer>
#include <QSerialPort>
#include <QUdpSocket>
#include <QFile>
#include <QElapsedTimer>

#include "qmodules.h"

class QNavApplication : public QCoreApplication
{
    Q_OBJECT

    qmodules m_modules;

    QUdpSocket m_socket;

    QString m_host;
    quint16 m_port;

    QTimer m_timer;
    int m_timer_interval;

    QFile m_logfile;

    bool m_console;

    bool m_navout_calculations;

    inputSettings_t m_inputSettings;

    QElapsedTimer m_elspseTimer;

public:
    QNavApplication(int &argc, char **argv);

    void set_license(quint64 key);
    bool is_licensed( void ) const;
    void set_timer_interval(int msecs);
    void set_port_host(const QString& host, quint16 port);
    void set_logfile(const QString& logfilename);
    void set_console(bool console);
    void set_navigation_calculations(bool status);
    void set_input_settings(const inputSettings_t& inputSettings);

    void start_gps(const QString& serialPortName, int baudrate = QSerialPort::Baud9600);
    void start_barometer(unsigned int bus, unsigned int dev);
    void start_imu(unsigned int bus, unsigned int dev);
    void start_serialout(const QString& serialPortName, int baudrate = QSerialPort::Baud9600);

    bool set_service(bool enable = true);

    bool run_qt_compile(const QString& workingdir);
    bool run_make_install(const QString& targetdir);

private slots:
    void onFinished(int code);
    void onTimerTimeout();
};

#endif // QNAVAPPLICATION_H
