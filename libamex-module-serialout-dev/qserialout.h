#ifndef QSERIALOUT_H
#define QSERIALOUT_H

#include <QObject>
#include <QtSerialPort/QSerialPort>

#include <map>
#include <string>

namespace amex {
    class QSerialOut : public QObject {

        Q_OBJECT

        QSerialPort m_serialPort;



    public:
        explicit QSerialOut(const QString& serialPortName, int baudrate = QSerialPort::Baud9600, QObject *parent = Q_NULLPTR);
        ~QSerialOut();


    signals:
        void dataReady();
        void finished(int code);

    private slots:
        void onReadyRead();

    public slots:
        void write(void* data, size_t size);
        void write(const char *data, qint64 len);
    }; // end of class QSerialOut
} // end of namespace amex



#endif // QSERIALOUT_H
