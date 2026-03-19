#include "NativeSerialPort.h"
#include <QDebug>
#ifndef _WIN32
#include <sys/select.h>
#endif

SerialReaderThread::SerialReaderThread(QObject* parent) : QThread(parent) {}

void SerialReaderThread::stop()
{
    m_stop.storeRelaxed(1);
}

#ifdef _WIN32

void SerialReaderThread::run()
{
    COMMTIMEOUTS timeouts = {};
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = MAXDWORD;
    timeouts.ReadTotalTimeoutConstant = 50;
    SetCommTimeouts(handle, &timeouts);

    char tmp[256];
    DWORD bytesRead = 0;

    while (!m_stop.loadRelaxed()) {
        bytesRead = 0;
        if (ReadFile(handle, tmp, sizeof(tmp), &bytesRead, nullptr) && bytesRead > 0) {
            QMutexLocker locker(&mutex);
            sharedBuffer.append(tmp, (int)bytesRead);
        }
    }
}

#else

#include <unistd.h>

void SerialReaderThread::run()
{
    char tmp[256];

    while (!m_stop.loadRelaxed()) {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        struct timeval tv = { 0, 50000 };

        int ret = select(fd + 1, &rfds, nullptr, nullptr, &tv);
        if (ret > 0) {
            ssize_t n = ::read(fd, tmp, sizeof(tmp));
            if (n > 0) {
                QMutexLocker locker(&mutex);
                sharedBuffer.append(tmp, (int)n);
            }
        }
    }
}

#endif

// Definitions communes (hors #ifdef)
NativeSerialPort::NativeSerialPort() {}

NativeSerialPort::~NativeSerialPort() { close(); }

void NativeSerialPort::setPortName(const QString& name) { m_portName = name; }
void NativeSerialPort::setBaudRate(int baudRate) { m_baudRate = baudRate; }
bool NativeSerialPort::isOpen() const { return m_isOpen; }

#ifdef _WIN32

bool NativeSerialPort::open(OpenMode mode)
{
    DWORD access = (mode == ReadWrite) ? (GENERIC_READ | GENERIC_WRITE) : GENERIC_READ;

    QString fullName = "\\\\.\\" + m_portName;
    HANDLE handle = CreateFileW(
        reinterpret_cast<LPCWSTR>(fullName.utf16()),
        access, 0, nullptr, OPEN_EXISTING, 0, nullptr
    );

    if (handle == INVALID_HANDLE_VALUE) {
        qDebug() << "NativeSerialPort: impossible d'ouvrir" << m_portName;
        return false;
    }

    DCB dcb = {};
    dcb.DCBlength = sizeof(DCB);
    if (!GetCommState(handle, &dcb)) { CloseHandle(handle); return false; }
    dcb.BaudRate = m_baudRate;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;
    if (!SetCommState(handle, &dcb)) { CloseHandle(handle); return false; }

    m_reader = new SerialReaderThread();
    m_reader->handle = handle;
    m_reader->start(QThread::LowestPriority);

    m_handle = handle;
    m_isOpen = true;
    return true;
}

void NativeSerialPort::close()
{
    if (m_reader) {
        m_reader->stop();
        if (m_reader->handle != INVALID_HANDLE_VALUE) {
            CloseHandle(m_reader->handle);
            m_reader->handle = INVALID_HANDLE_VALUE;
        }
        m_reader->wait();
        delete m_reader;
        m_reader = nullptr;
    }
    m_handle = INVALID_HANDLE_VALUE;
    m_isOpen = false;
    m_buffer.clear();
}

#else

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

static speed_t toBaudConstant(int baud)
{
    switch (baud) {
    case 9600:   return B9600;
    case 19200:  return B19200;
    case 38400:  return B38400;
    case 57600:  return B57600;
    case 115200: return B115200;
    case 230400: return B230400;
    default:     return B9600;
    }
}

bool NativeSerialPort::open(OpenMode mode)
{
    int flags = (mode == ReadWrite) ? O_RDWR : O_RDONLY;
    flags |= O_NOCTTY | O_NONBLOCK;

    int fd = ::open(m_portName.toLocal8Bit().constData(), flags);
    if (fd < 0) {
        qDebug() << "NativeSerialPort: impossible d'ouvrir" << m_portName;
        return false;
    }

    struct termios tty = {};
    if (tcgetattr(fd, &tty) != 0) { ::close(fd); return false; }

    speed_t speed = toBaudConstant(m_baudRate);
    cfsetispeed(&tty, speed);
    cfsetospeed(&tty, speed);
    cfmakeraw(&tty);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) { ::close(fd); return false; }

    m_reader = new SerialReaderThread();
    m_reader->fd = fd;
    m_reader->start(QThread::LowestPriority);

    m_fd = fd;
    m_isOpen = true;
    return true;
}

void NativeSerialPort::close()
{
    if (m_reader) {
        m_reader->stop();
        m_reader->wait();
        ::close(m_reader->fd);
        delete m_reader;
        m_reader = nullptr;
    }
    m_fd = -1;
    m_isOpen = false;
    m_buffer.clear();
}

#endif

void NativeSerialPort::pullFromReader()
{
    if (!m_reader) return;
    QMutexLocker locker(&m_reader->mutex);
    if (!m_reader->sharedBuffer.isEmpty()) {
        m_buffer.append(m_reader->sharedBuffer);
        m_reader->sharedBuffer.clear();
    }
}

bool NativeSerialPort::canReadLine()
{
    pullFromReader();
    return m_buffer.contains('\n');
}

QByteArray NativeSerialPort::readLine()
{
    pullFromReader();
    int idx = m_buffer.indexOf('\n');
    if (idx < 0) return QByteArray();

    QByteArray line = m_buffer.left(idx + 1);
    m_buffer.remove(0, idx + 1);
    return line;
}

bool NativeSerialPort::write(const QByteArray& data)
{
    if (!m_isOpen || data.isEmpty()) return false;

#ifdef _WIN32
    if (m_handle == INVALID_HANDLE_VALUE) return false;
    {
        const char* ptr = data.constData();
        DWORD remaining = (DWORD)data.size();
        while (remaining > 0) {
            DWORD bytesWritten = 0;
            if (!WriteFile(m_handle, ptr, remaining, &bytesWritten, nullptr))
                return false;
            ptr += bytesWritten;
            remaining -= bytesWritten;
        }
        FlushFileBuffers(m_handle);
    }
    return true;
#else
    if (m_fd < 0) return false;

    const char* ptr = data.constData();
    int remaining = data.size();
    const int maxRetries = 20;

    for (int retry = 0; remaining > 0 && retry < maxRetries; ++retry) {
        ssize_t n = ::write(m_fd, ptr, remaining);
        if (n > 0) {
            ptr += n;
            remaining -= (int)n;
            retry = 0;
        }
        else if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            struct timespec ts = { 0, 2000000 };
            nanosleep(&ts, nullptr);
        }
        else {
            return false;
        }
    }

    if (remaining > 0) return false;

    tcdrain(m_fd);
    return true;
#endif
}

static const char* ARDUINO_KEYWORDS[] = {
    "Arduino", "CH340", "CH341", "CP210", "FTDI", "FT232", "USB Serial", "USB de S", nullptr
};

#ifdef _WIN32

static const struct { const char* vid; const char* pid; } ARDUINO_IDS[] = {
    { "2341", "0042" },
    { "2341", "0010" },
    { "2341", "0043" },
    { "2341", "0001" },
    { "2341", "0036" },
    { "2341", "003B" },
    { "2341", "0069" },
    { "1A86", "7523" },
    { "1A86", "5523" },
    { "10C4", "EA60" },
    { "0403", "6001" },
    { nullptr, nullptr }
};

QString NativeSerialPort::findArduinoPort()
{
    HDEVINFO devInfo = SetupDiGetClassDevs(
        &GUID_DEVCLASS_PORTS, nullptr, nullptr, DIGCF_PRESENT
    );
    if (devInfo == INVALID_HANDLE_VALUE) return QString();

    SP_DEVINFO_DATA devData;
    devData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(devInfo, i, &devData); ++i) {

        char hardwareId[512] = {};
        SetupDiGetDeviceRegistryPropertyA(
            devInfo, &devData, SPDRP_HARDWAREID,
            nullptr, (PBYTE)hardwareId, sizeof(hardwareId), nullptr
        );

        QString hwId = QString::fromLatin1(hardwareId).toUpper();

        bool isArduino = false;
        for (int k = 0; ARDUINO_IDS[k].vid; ++k) {
            QString vidStr = QString("VID_%1").arg(ARDUINO_IDS[k].vid).toUpper();
            QString pidStr = QString("PID_%1").arg(ARDUINO_IDS[k].pid).toUpper();
            if (hwId.contains(vidStr) && hwId.contains(pidStr)) {
                isArduino = true;
                break;
            }
        }

        if (isArduino) {
            char friendlyName[256] = {};
            SetupDiGetDeviceRegistryPropertyA(
                devInfo, &devData, SPDRP_FRIENDLYNAME,
                nullptr, (PBYTE)friendlyName, sizeof(friendlyName), nullptr
            );

            char* start = strrchr(friendlyName, '(');
            char* end = strrchr(friendlyName, ')');
            if (start && end && end > start) {
                QString port = QString::fromLatin1(start + 1, (int)(end - start - 1));
                SetupDiDestroyDeviceInfoList(devInfo);
                return port;
            }
        }
    }

    SetupDiDestroyDeviceInfoList(devInfo);
    return QString();
}

#else

#include <QDir>
#include <QFile>

QString NativeSerialPort::findArduinoPort()
{
    const QStringList prefixes = { "ttyUSB", "ttyACM" };

    for (const QString& prefix : prefixes) {
        QDir dir("/sys/class/tty");
        const QStringList entries = dir.entryList(QStringList(prefix + "*"), QDir::Dirs | QDir::System);

        for (const QString& entry : entries) {
            QString mfgPath = QString("/sys/class/tty/%1/device/../manufacturer").arg(entry);
            QFile mfgFile(mfgPath);
            if (mfgFile.open(QIODevice::ReadOnly)) {
                QString mfg = QString::fromUtf8(mfgFile.readAll()).trimmed();
                for (int k = 0; ARDUINO_KEYWORDS[k]; ++k) {
                    if (mfg.contains(ARDUINO_KEYWORDS[k], Qt::CaseInsensitive))
                        return "/dev/" + entry;
                }
            }
            return "/dev/" + entry;
        }
    }

    return QString();
}

#endif