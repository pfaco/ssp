/*
 * MIT License
 *
 * Copyright (c) 2018 Paulo Faco (paulofaco@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

///@file

#include <ssp/serial.h>
#include <sstream>
#include <windows.h>

namespace ssp
{

class SerialPort::impl
{
    HANDLE m_hserial = NULL;
    Baudrate m_baud;
    Parity   m_parity;
    Databits m_dbits;
    Stopbits m_sbits;
    unsigned m_timeout_ms;
    unsigned m_inter_byte_timeout_ms = 5;

public:
    static auto available_ports() -> std::vector<SerialInfo>
    {
        std::vector<SerialInfo> retval;
        for (unsigned i = 0; i <= 255; ++i) {

        }
        return retval;
    }

    impl(std::string const &id,
         Baudrate baud,
         Parity par,
         Databits dbits,
         Stopbits sbits,
         unsigned timeout) :
             m_baud{baud},
             m_parity{par},
             m_dbits{dbits},
             m_sbits{sbits},
             m_timeout_ms{timeout}
    {
        std::string long_prefix = R"(\\.\)";
        std::string prefix = "COM";
        std::string port_name = id;
        if ( (id.compare(0, prefix.size(), prefix) == 0) && (id.size() > 4) ) {
            port_name.insert(0, long_prefix);
        }

        m_hserial = CreateFile(port_name.c_str(),
                               GENERIC_READ | GENERIC_WRITE,
                               0,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);

        if (m_hserial == INVALID_HANDLE_VALUE) {
            throw SerialErrorOpening{};
        }

        configure_port();
    }

    ~impl()
    {
        CloseHandle(m_hserial); //Closing the Serial Port
    }

    void set_params(Baudrate baud, Parity par, Databits dbits, Stopbits sbits, unsigned timeout_ms)
    {
        m_baud = baud;
        m_parity = par;
        m_dbits = dbits;
        m_sbits = sbits;
        m_timeout_ms = timeout_ms;
        configure_port();
    }

    void set_baud(Baudrate baud)
    {
        m_baud = baud;
        configure_port();
    }

    void set_parity(Parity par)
    {
        m_parity = par;
        configure_port();
    }

    void set_databits(Databits dbits)
    {
        m_dbits = dbits;
        configure_port();
    }

    void set_stopbits(Stopbits sbits)
    {
        m_sbits = sbits;
        configure_port();
    }

    void set_timeout(unsigned timeout_ms)
    {
        m_timeout_ms = timeout_ms;
        configure_timeout();
    }

    auto write(std::vector<uint8_t> const &data) -> size_t
    {
        DWORD n_bytes_writen = 0;
        if (!WriteFile(m_hserial, &data[0], data.size(), &n_bytes_writen, NULL)) {
            throw SerialErrorIO();
        }
        return static_cast<size_t>(n_bytes_writen);
    }

    auto available() -> size_t
    {
        COMSTAT comstat;
        if (!ClearCommError(m_hserial, NULL, &comstat)) {
            throw SerialErrorIO();
        }
        return comstat.cbInQue;
    }

    auto read() -> std::vector<uint8_t>
    {
        std::vector<uint8_t> retval;
        read(retval);
        return retval;
    }

    void read(std::vector<uint8_t> &buffer)
    {
        uint8_t temp[512];
        DWORD amount_read;

        if (!ReadFile(m_hserial, temp, sizeof(temp), &amount_read, NULL)) {
            throw SerialErrorIO();
        }

        for (auto i = 0; i < amount_read; ++i) {
            buffer.push_back(temp[i]);
        }
    }

private:
    void configure_timeout()
    {
        COMMTIMEOUTS com_timeout = {0};
        if (!GetCommTimeouts(m_hserial,&com_timeout)) {
            throw SerialErrorConfig();
        }

        com_timeout.ReadIntervalTimeout = m_inter_byte_timeout_ms;
        com_timeout.ReadTotalTimeoutConstant = m_timeout_ms;
        com_timeout.ReadTotalTimeoutMultiplier = 0;

        if (!SetCommTimeouts(m_hserial, &com_timeout)) {
            throw SerialErrorConfig();
        }
    }

    void configure_port()
    {
        DCB dcb_params = {0};
        dcb_params.DCBlength = sizeof(dcb_params);
        if (!GetCommState(m_hserial, &dcb_params)) {
            throw SerialErrorConfig();
        }

        dcb_params.fOutxCtsFlow = false;
        dcb_params.fRtsControl = 0x00;
        dcb_params.fOutX = false;
        dcb_params.fInX = false;

        dcb_params.BaudRate = static_cast<unsigned>(m_baud);
        m_inter_byte_timeout_ms= static_cast<unsigned>(m_baud)/8;
        m_inter_byte_timeout_ms = 1200/m_inter_byte_timeout_ms;
        m_inter_byte_timeout_ms = m_inter_byte_timeout_ms < 5 ? 5 : m_inter_byte_timeout_ms;
        configure_timeout();

        switch (m_parity) {
            case Parity::EVEN:
                dcb_params.Parity = EVENPARITY;
                break;
            case Parity::ODD:
                dcb_params.Parity = ODDPARITY;
                break;
            case Parity::NONE:
                dcb_params.Parity = NOPARITY;
                break;
            case Parity::MARK:
                dcb_params.Parity = MARKPARITY;
                break;
            case Parity::SPACE:
                dcb_params.Parity = SPACEPARITY;
                break;
        }

        switch (m_dbits) {
            case Databits::_5:
                dcb_params.ByteSize = 5;
                break;
            case Databits::_6:
                dcb_params.ByteSize = 6;
                break;
            case Databits::_7:
                dcb_params.ByteSize = 7;
                break;
            case Databits::_8:
                dcb_params.ByteSize = 8;
                break;
        }

        switch (m_sbits) {
            case Stopbits::_1:
                dcb_params.StopBits = ONESTOPBIT;
                break;
            case Stopbits::_1POINT5:
                dcb_params.StopBits = ONE5STOPBITS;
                break;
            case Stopbits::_2:
                dcb_params.StopBits = TWOSTOPBITS;
                break;
        }

        if (!SetCommState(m_hserial, &dcb_params)) {
            throw SerialErrorConfig();
        }
    }

};

auto SerialPort::available_ports() -> std::vector<SerialInfo>
{
    return impl::available_ports();
}

SerialPort::SerialPort(std::string const &id, Baudrate baud, Parity par, Databits dbits, Stopbits sbits, unsigned timeout_ms) :
                         m_pimpl{std::make_unique<impl>(id, baud, par, dbits, sbits, timeout_ms)} {};

SerialPort::~SerialPort() = default;

void SerialPort::set_params(Baudrate baud, Parity par, Databits dbits, Stopbits sbits, unsigned timeout_ms)
{
    m_pimpl->set_params(baud, par, dbits, sbits, timeout_ms);
}

auto SerialPort::write(std::vector<uint8_t> const &data) -> size_t
{
    return m_pimpl->write(data);
}

auto SerialPort::available() -> size_t
{
    return m_pimpl->available();
}

auto SerialPort::read() -> std::vector<uint8_t>
{
    return m_pimpl->read();
}

void SerialPort::read(std::vector<uint8_t> &buffer)
{
    m_pimpl->read(buffer);
}

void SerialPort::set_baud(Baudrate baud)
{
    m_pimpl->set_baud(baud);
}

void SerialPort::set_parity(Parity par)
{
    m_pimpl->set_parity(par);
}

void SerialPort::set_databits(Databits dbits)
{
    m_pimpl->set_databits(dbits);
}

void SerialPort::set_stopbits(Stopbits sbits)
{
    m_pimpl->set_stopbits(sbits);
}

void SerialPort::set_timeout(unsigned timeout_ms)
{
    m_pimpl->set_timeout(timeout_ms);
}

}