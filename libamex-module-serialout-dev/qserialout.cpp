#include "qserialout.h"

#include <QDebug>
#include <QProcess>
#include <QFile>

using namespace amex;

QSerialOut::QSerialOut(const QString& serialPortName, int baudrate /*= QSerialPort::Baud9600*/, QObject *parent /*= Q_NULLPTR*/)
    : QObject(parent)
{


    m_serialPort.setPortName(serialPortName);
    m_serialPort.setBaudRate(baudrate);

    QObject::connect(&m_serialPort, SIGNAL(readyRead()), this, SLOT(onReadyRead()));


    if(!QFile::exists(serialPortName)) {
        QProcess process;
        process.start("sudo sh -c \"echo BB-UART4 > /sys/devices/platform/bone_capemgr/slots\"");
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

QSerialOut::~QSerialOut()
{
    if(m_serialPort.isOpen()) {
        m_serialPort.close();
    }
}

void QSerialOut::onReadyRead()
{
    //m_serialPort.readAll();
}

void QSerialOut::write(void *data, size_t size)
{
    m_serialPort.write(static_cast<const char*>(data), static_cast<qint64>(size));
}

void QSerialOut::write(const char *data, qint64 len)
{
    m_serialPort.write(data, len);
}

