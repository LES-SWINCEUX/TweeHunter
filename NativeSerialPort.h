#ifndef NATIVESERIALPORT_H
#define NATIVESERIALPORT_H

#include <QString>
#include <QByteArray>
#include <QMutex>
#include <QThread>
#include <QAtomicInt>

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

class SerialReaderThread : public QThread
{
public:
    explicit SerialReaderThread(QObject* parent = nullptr);
    void stop();

    QByteArray  sharedBuffer;
    QMutex      mutex;

#ifdef _WIN32
    HANDLE      handle = INVALID_HANDLE_VALUE;
#else
    int         fd = -1;
#endif

protected:
    void run() override;

private:
    QAtomicInt  m_stop{ 0 };
};

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

    bool write(const QByteArray& data);

private:
    void pullFromReader();

    QString m_portName;
    int m_baudRate = 115200;
    bool m_isOpen = false;
    QByteArray m_buffer;
    SerialReaderThread* m_reader = nullptr;

#ifdef _WIN32
    HANDLE m_handle = INVALID_HANDLE_VALUE;
#else
    int m_fd = -1;
#endif
};

#endif