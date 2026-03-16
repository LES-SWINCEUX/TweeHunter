#include "NativeSerialPort.h"
#include <QDebug>

#ifdef _WIN32

NativeSerialPort::NativeSerialPort() {}

NativeSerialPort::~NativeSerialPort() { close(); }

void NativeSerialPort::setPortName(const QString& name) { m_portName = name; }
void NativeSerialPort::setBaudRate(int baudRate) { m_baudRate = baudRate; }
bool NativeSerialPort::isOpen() const { return m_isOpen; }

bool NativeSerialPort::open(OpenMode mode)
{
    DWORD access = (mode == ReadWrite) ? (GENERIC_READ | GENERIC_WRITE) : GENERIC_READ;

    QString fullName = "\\\\.\\" + m_portName;
    m_handle = CreateFileW(
        reinterpret_cast<LPCWSTR>(fullName.utf16()),
        access, 0, nullptr, OPEN_EXISTING, 0, nullptr
    );

    if (m_handle == INVALID_HANDLE_VALUE) {
        qDebug() << "NativeSerialPort: impossible d'ouvrir" << m_portName;
        return false;
    }

    DCB dcb = {};
    dcb.DCBlength = sizeof(DCB);
    if (!GetCommState(m_handle, &dcb)) {
        CloseHandle(m_handle);
        m_handle = INVALID_HANDLE_VALUE;
        return false;
    }
    dcb.BaudRate = m_baudRate;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;
    if (!SetCommState(m_handle, &dcb)) {
        CloseHandle(m_handle);
        m_handle = INVALID_HANDLE_VALUE;
        return false;
    }

    COMMTIMEOUTS timeouts = {};
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 0;
    SetCommTimeouts(m_handle, &timeouts);

    m_isOpen = true;
    return true;
}

void NativeSerialPort::close()
{
    if (m_handle != INVALID_HANDLE_VALUE) {
        CloseHandle(m_handle);
        m_handle = INVALID_HANDLE_VALUE;
    }
    m_isOpen = false;
    m_buffer.clear();
}

void NativeSerialPort::readIntoBuffer()
{
    if (!m_isOpen) return;

    char tmp[256];
    DWORD bytesRead = 0;
    while (ReadFile(m_handle, tmp, sizeof(tmp), &bytesRead, nullptr) && bytesRead > 0)
        m_buffer.append(tmp, bytesRead);
}

bool NativeSerialPort::write(const QByteArray& data)
{
    if (!m_isOpen) return false;
    DWORD written = 0;
    return WriteFile(m_handle, data.constData(), data.size(), &written, nullptr);
}

#else

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

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

NativeSerialPort::NativeSerialPort() {}

NativeSerialPort::~NativeSerialPort() { close(); }

void NativeSerialPort::setPortName(const QString& name) { m_portName = name; }
void NativeSerialPort::setBaudRate(int baudRate) { m_baudRate = baudRate; }
bool NativeSerialPort::isOpen() const { return m_isOpen; }

bool NativeSerialPort::open(OpenMode mode)
{
    int flags = (mode == ReadWrite) ? O_RDWR : O_RDONLY;
    flags |= O_NOCTTY | O_NONBLOCK;

    m_fd = ::open(m_portName.toLocal8Bit().constData(), flags);
    if (m_fd < 0) {
        qDebug() << "NativeSerialPort: impossible d'ouvrir" << m_portName;
        return false;
    }

    struct termios tty = {};
    if (tcgetattr(m_fd, &tty) != 0) {
        ::close(m_fd);
        m_fd = -1;
        return false;
    }

    speed_t speed = toBaudConstant(m_baudRate);
    cfsetispeed(&tty, speed);
    cfsetospeed(&tty, speed);

    cfmakeraw(&tty);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(m_fd, TCSANOW, &tty) != 0) {
        ::close(m_fd);
        m_fd = -1;
        return false;
    }

    m_isOpen = true;
    return true;
}

void NativeSerialPort::close()
{
    if (m_fd >= 0) {
        ::close(m_fd);
        m_fd = -1;
    }
    m_isOpen = false;
    m_buffer.clear();
}

void NativeSerialPort::readIntoBuffer()
{
    if (!m_isOpen) return;

    char tmp[256];
    ssize_t n;
    while ((n = ::read(m_fd, tmp, sizeof(tmp))) > 0)
        m_buffer.append(tmp, (int)n);
}

bool NativeSerialPort::write(const QByteArray& data)
{
    if (!m_isOpen) return false;
    return ::write(m_fd, data.constData(), data.size()) > 0;
}

#endif

bool NativeSerialPort::canReadLine()
{
    readIntoBuffer();
    return m_buffer.contains('\n');
}

QByteArray NativeSerialPort::readLine()
{
    readIntoBuffer();
    int idx = m_buffer.indexOf('\n');
    if (idx < 0) return QByteArray();

    QByteArray line = m_buffer.left(idx + 1);
    m_buffer.remove(0, idx + 1);
    return line;
}

static const char* ARDUINO_KEYWORDS[] = {
    "Arduino", "CH340", "CH341", "CP210", "FTDI", "FT232", "USB Serial", nullptr
};

#ifdef _WIN32

QString NativeSerialPort::findArduinoPort()
{
    HDEVINFO devInfo = SetupDiGetClassDevs(
        &GUID_DEVCLASS_PORTS, nullptr, nullptr, DIGCF_PRESENT
    );
    if (devInfo == INVALID_HANDLE_VALUE) return QString();

    SP_DEVINFO_DATA devData;
    devData.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(devInfo, i, &devData); ++i) {

        char friendlyName[256] = {};
        SetupDiGetDeviceRegistryPropertyA(
            devInfo, &devData, SPDRP_FRIENDLYNAME,
            nullptr, (PBYTE)friendlyName, sizeof(friendlyName), nullptr
        );

        qDebug() << "Port detecte:" << friendlyName;

        bool isArduino = false;
        for (int k = 0; ARDUINO_KEYWORDS[k]; ++k) {
            if (strstr(friendlyName, ARDUINO_KEYWORDS[k])) {
                isArduino = true;
                break;
            }
        }

        if (isArduino) {
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
                    if (mfg.contains(ARDUINO_KEYWORDS[k], Qt::CaseInsensitive)) {
                        return "/dev/" + entry;
                    }
                }
            }
            return "/dev/" + entry;
        }
    }

    return QString();
}

#endif