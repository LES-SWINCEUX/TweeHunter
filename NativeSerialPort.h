#ifndef NATIVESERIALPORT_H
#define NATIVESERIALPORT_H

#include <QString>
#include <QByteArray>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#else
#include <termios.h>
#endif

class NativeSerialPort
{
public:
    enum OpenMode { ReadOnly = 0, ReadWrite = 1 };

    NativeSerialPort();
    ~NativeSerialPort();

    static QString findArduinoPort();

    void setPortName(const QString& name);
    void setBaudRate(int baudRate);

    bool open(OpenMode mode);
    void close();
    bool isOpen() const;

    bool canReadLine();

    QByteArray readLine();

private:
    void readIntoBuffer();

    QString     m_portName;
    int         m_baudRate = 115200;
    bool        m_isOpen = false;
    QByteArray  m_buffer;

#ifdef _WIN32
    HANDLE      m_handle = INVALID_HANDLE_VALUE;
#else
    int         m_fd = -1;
#endif
};

#endif