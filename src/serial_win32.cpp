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
    HANDLE hserial_ = NULL;
    Baudrate baud_;
    Parity   parity_;
    Databits dbits_;
    Stopbits sbits_;
    unsigned timeout_ms_;
    unsigned inter_byte_timeout_ms_ = 50;
    std::function<void(const std::vector<uint8_t>&)> rx_listener = nullptr;
    std::function<void(const std::vector<uint8_t>&)> tx_listener = nullptr;

public:
    static auto available_ports() -> std::vector<SerialInfo>
    {
		auto retval = std::vector<SerialInfo>{};
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
             baud_{baud},
             parity_{par},
             dbits_{dbits},
             sbits_{sbits},
             timeout_ms_{timeout}
	{
		auto long_prefix = std::string{R"(\\.\)"};
		auto prefix = std::string{ "COM" };
		auto port_name = std::string{ id };

        if ( (id.compare(0, prefix.size(), prefix) == 0) && (id.size() > 4) ) {
            port_name.insert(0, long_prefix);
        }

        hserial_ = CreateFile(port_name.c_str(),
                               GENERIC_READ | GENERIC_WRITE,
                               0,
                               NULL,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);

        if (hserial_ == INVALID_HANDLE_VALUE) {
            throw SerialErrorOpening{};
        }

        configure_port();
    }

    ~impl()
    {
        CloseHandle(hserial_); //Closing the Serial Port
    }

    void install_rx_listener(std::function<void(const std::vector<uint8_t>&)> func) {
        rx_listener = func;
    }

    void install_tx_listener(std::function<void(const std::vector<uint8_t>&)> func) {
        tx_listener = func;
    }

    void set_params(Baudrate baud, Parity par, Databits dbits, Stopbits sbits, unsigned timeout_ms)
    {
        baud_ = baud;
        parity_ = par;
        dbits_ = dbits;
        sbits_ = sbits;
        timeout_ms_ = timeout_ms;
        configure_port();
    }

    void set_baud(Baudrate baud)
    {
        baud_ = baud;
        configure_port();
    }

    void set_parity(Parity par)
    {
        parity_ = par;
        configure_port();
    }

    void set_databits(Databits dbits)
    {
        dbits_ = dbits;
        configure_port();
    }

    void set_stopbits(Stopbits sbits)
    {
        sbits_ = sbits;
        configure_port();
    }

    void set_timeout(unsigned timeout_ms)
    {
        timeout_ms_ = timeout_ms;
        configure_timeout();
    }

    auto write(std::vector<uint8_t> const &data) -> size_t
    {
        DWORD n_bytes_writen = 0;
        if (!WriteFile(hserial_, &data[0], data.size(), &n_bytes_writen, NULL)) {
            throw SerialErrorIO();
        }
        if (tx_listener != nullptr) {
            tx_listener(data);
        }
        return static_cast<size_t>(n_bytes_writen);
    }

    auto available() -> size_t
    {
        COMSTAT comstat;
        if (!ClearCommError(hserial_, NULL, &comstat)) {
            throw SerialErrorIO();
        }
        return comstat.cbInQue;
    }

    auto read() -> std::vector<uint8_t>
    {
        std::vector<uint8_t> retval;
        read(retval);
        if (rx_listener != nullptr) {
            rx_listener(retval);
        }
        return retval;
    }

    void read(std::vector<uint8_t> &buffer)
    {
        uint8_t temp[512];
        DWORD amount_read;

        if (!ReadFile(hserial_, temp, sizeof(temp), &amount_read, NULL)) {
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
        if (!GetCommTimeouts(hserial_,&com_timeout)) {
            throw SerialErrorConfig();
        }

        com_timeout.ReadIntervalTimeout = inter_byte_timeout_ms_;
        com_timeout.ReadTotalTimeoutConstant = timeout_ms_;
        com_timeout.ReadTotalTimeoutMultiplier = 0;

        if (!SetCommTimeouts(hserial_, &com_timeout)) {
            throw SerialErrorConfig();
        }
    }

    void configure_port()
    {
        DCB dcb_params = {0};
        dcb_params.DCBlength = sizeof(dcb_params);
        if (!GetCommState(hserial_, &dcb_params)) {
            throw SerialErrorConfig();
        }

        dcb_params.fOutxCtsFlow = false;
        dcb_params.fRtsControl = 0x00;
        dcb_params.fOutX = false;
        dcb_params.fInX = false;

        dcb_params.BaudRate = static_cast<unsigned>(baud_);
        inter_byte_timeout_ms_= static_cast<unsigned>(baud_)/8;
        inter_byte_timeout_ms_ = 1200/inter_byte_timeout_ms_;
        inter_byte_timeout_ms_ = inter_byte_timeout_ms_ < 50 ? 50 : inter_byte_timeout_ms_;
        configure_timeout();

        switch (parity_) {
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

        switch (dbits_) {
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

        switch (sbits_) {
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

        if (!SetCommState(hserial_, &dcb_params)) {
            throw SerialErrorConfig();
        }
    }

};

auto SerialPort::available_ports() -> std::vector<SerialInfo>
{
    return impl::available_ports();
}

SerialPort::SerialPort(std::string const &id, Baudrate baud, Parity par, Databits dbits, Stopbits sbits, unsigned timeout_ms) :
                         pimpl_{std::make_unique<impl>(id, baud, par, dbits, sbits, timeout_ms)} {};

SerialPort::~SerialPort() = default;

void SerialPort::set_params(Baudrate baud, Parity par, Databits dbits, Stopbits sbits, unsigned timeout_ms)
{
    pimpl_->set_params(baud, par, dbits, sbits, timeout_ms);
}

auto SerialPort::write(std::vector<uint8_t> const &data) -> size_t
{
    return pimpl_->write(data);
}

auto SerialPort::available() -> size_t
{
    return pimpl_->available();
}

auto SerialPort::read() -> std::vector<uint8_t>
{
    return pimpl_->read();
}

void SerialPort::read(std::vector<uint8_t> &buffer)
{
    pimpl_->read(buffer);
}

void SerialPort::set_baud(Baudrate baud)
{
    pimpl_->set_baud(baud);
}

void SerialPort::set_parity(Parity par)
{
    pimpl_->set_parity(par);
}

void SerialPort::set_databits(Databits dbits)  
{
    pimpl_->set_databits(dbits);
}

void SerialPort::set_stopbits(Stopbits sbits)
{
    pimpl_->set_stopbits(sbits);
}

void SerialPort::set_timeout(unsigned timeout_ms)
{
    pimpl_->set_timeout(timeout_ms);
}

void SerialPort::install_rx_listener(std::function<void(const std::vector<uint8_t>&)> func) {
    pimpl_->install_rx_listener(func);
}

void SerialPort::install_tx_listener(std::function<void(const std::vector<uint8_t>&)> func) {
    pimpl_->install_tx_listener(func);
}

}
