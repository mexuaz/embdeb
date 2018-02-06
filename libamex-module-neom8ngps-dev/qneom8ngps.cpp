#include "qneom8ngps.h"

#include <QDebug>
#include <QProcess>
#include <QFile>

using namespace amex;

QNeoM8nGPS::QNeoM8nGPS(const QString& serialPortName, int baudrate /*= QSerialPort::Baud115200*/, QObject *parent /*= Q_NULLPTR*/)
    : QObject(parent)
{
/*
    m_fieldNames[nmea_sen_t::GPGSV] = "GPGSV";
    m_fieldNames[nmea_sen_t::GLGSV] = "GLGSV";
    m_fieldNames[nmea_sen_t::GNGLL] = "GNGLL";
    m_fieldNames[nmea_sen_t::GNRMC] = "GNRMC";
    m_fieldNames[nmea_sen_t::GNVTG] = "GNVTG";
    m_fieldNames[nmea_sen_t::GNGGA] = "GNGGA";
    m_fieldNames[nmea_sen_t::GNGSA] = "GNGSA";
*/

    m_serialPort.setPortName(serialPortName);
    m_serialPort.setBaudRate(baudrate);

    QObject::connect(&m_serialPort, SIGNAL(readyRead()), this, SLOT(onReadyRead()));


    if(!QFile::exists(serialPortName)) {
        QProcess process;
        process.start("sudo sh -c \"echo BB-UART1 > /sys/devices/platform/bone_capemgr/slots\"");
        process.waitForFinished(-1);
        QByteArray err = process.readAllStandardError();
        if(!err.isEmpty()) {
            qCritical() << err;
            emit finished(1);
        }
         if(!QFile::exists(serialPortName)) {
             qCritical() << "The serial port name is not valid: " << serialPortName;
             emit finished(1);
         }
    }




    QProcess process;
    process.start("sudo chmod 666 " + serialPortName);
    process.waitForFinished(-1);
    QByteArray err = process.readAllStandardError();
    if(!err.isEmpty()) {
        qCritical() << err;
        emit finished(1);
    }


    if(!m_serialPort.open(QIODevice::ReadWrite)) {
        qCritical() << m_serialPort.errorString();
        emit finished(1);
    }
}

QNeoM8nGPS::~QNeoM8nGPS()
{
    if(m_serialPort.isOpen()) {
        m_serialPort.close();
    }
}

void QNeoM8nGPS::get_info(nmea_t & nmea) {
    nmea = m_nmea;
}

void QNeoM8nGPS::onReadyRead()
{
    static QString data;
    data.append( m_serialPort.readAll() );

    int pos = data.indexOf('\n', 0);
    /*
     * Process only when we have newline character in the received data
     * and remove the line that will be processed.
     */
    while(pos != -1) { // process only when we have newline character in the
        QString line(data.left(pos));
        data.remove(0, pos+1);


/*
        for(size_t i = 0; i < m_fieldNames.size(); i++ ) { // Search for the matching NEMA Field
            nmea_sen_t nmeatype = static_cast<nmea_sen_t>(i);
            std::string val("$"+m_fieldNames[nmeatype]+",");
            if(line.startsWith(val.c_str())) {
                int len = static_cast<int>(val.size());
                int pos = line.indexOf('*');
                m_nmea[nmeatype] =
                        line.mid(len, pos - len).toStdString();
                break;
            } // End of if
        } // End of for
*/
        if(line.startsWith('$')) {
            int commaPos = line.indexOf(',');
            QString fieldname = line.mid(1, commaPos-1);
            int startPos = line.indexOf('*');
            m_nmea[fieldname.toStdString()] =
                    line.mid(commaPos+1, startPos - commaPos -1).toStdString();
        } // End of if


        pos = data.indexOf('\n', 0);
    } // End of while
}

void QNeoM8nGPS::dump() {
    for(const auto& m : m_nmea) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
        qDebug().noquote().nospace() << m.first.c_str() << ": " << m.second.c_str();
#else
        qDebug() << qPrintable(std::string( m.first + ": " + m.second ).c_str());
#endif
    }
}
