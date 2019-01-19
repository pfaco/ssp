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

#include "ssp/serial.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <cstring>
#include <unistd.h>
#include <chrono>

namespace ssp
{
    struct SerialPort::impl {

        int fd; //file descriptor
        unsigned timeout_ms_;
        std::function<void(const std::vector<uint8_t>&)> rx_listener = nullptr;
        std::function<void(const std::vector<uint8_t>&)> tx_listener = nullptr;

        impl(std::string const &id,
             Baudrate baud,
             Parity par,
             Databits dbits,
             Stopbits sbits,
             unsigned timeout_ms)
        {
            if ((fd = open(id.c_str(), O_RDWR | O_NOCTTY)) < 0) {
                throw SerialErrorOpening();
            }
            set_params(Baudrate::_9600, Parity::NONE, Databits::_8, Stopbits::_1, timeout_ms);
        }

        ~impl()
        {
            close(fd);
        }

        void install_rx_listener(std::function<void(const std::vector<uint8_t>&)> func) {
            rx_listener = func;
        }

        void install_tx_listener(std::function<void(const std::vector<uint8_t>&)> func) {
            tx_listener = func;
        }

        static auto available_ports() -> std::vector<SerialInfo>
        {
            std::vector<SerialInfo> retval;
            return retval;
        }

        void set_params(Baudrate baud, Parity par, Databits dbits, Stopbits sbits, unsigned timeout_ms)
        {
            struct termios params;
            memset(&params, 0, sizeof(params));

            timeout_ms_ = timeout_ms;

            //configure input flags
            params.c_iflag = IGNPAR |   //
                             ICRNL;     //

            //configure control flags
            params.c_cflag = CREAD |    //enables receive
                             CLOCAL |   //ignores modem control lines
                             CRTSCTS;   //enables RTS/CTS control

            params.c_lflag = 0; //non-canonical, no echo
            params.c_oflag = 0; //raw output

            switch(baud) {
                case Baudrate::_110:
                    params.c_cflag |= B110;
                    break;
                case Baudrate::_300:
                    params.c_cflag |= B300;
                    break;
                case Baudrate::_9600:
                    params.c_cflag |= B9600;
                    break;
            }

            switch (par) {
                case Parity::NONE:
                    params.c_cflag &= ~PARENB;
                    break;
                case Parity::EVEN:
                    params.c_cflag |= PARENB;
                    break;
                case Parity::ODD:
                    params.c_cflag |= PARENB;
                    params.c_cflag |= PARODD;
                    break;
            }

            switch (dbits) {
                case Databits::_5:
                    params.c_cflag |= CS5;
                    break;
                case Databits::_6:
                    params.c_cflag |= CS6;
                    break;
                case Databits::_7:
                    params.c_cflag |= CS7;
                    break;
                case Databits::_8:
                    params.c_cflag |= CS8;
                    break;
            }

            switch (sbits) {
                case Stopbits::_1:
                    params.c_cflag &= ~CSTOPB;
                    break;
                case Stopbits::_1POINT5:
                    params.c_cflag |= CSTOPB;
                    break;
                case Stopbits::_2:
                    params.c_cflag |= CSTOPB;
                    break;
            }

            tcsetattr(fd, TCSANOW, &params);
        }

        auto write(std::vector<uint8_t> const &data) -> size_t
        {
            auto written = ::write(fd, data.data(), data.size());
            tcflush(fd, TCIFLUSH);
            if (tx_listener != nullptr) {
                tx_listener(data);
            }
            return written;
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
            uint8_t temp[128];
            auto start = std::chrono::steady_clock::now();
            do {
                auto res = ::read(fd, temp, sizeof(temp));
                for (auto i = 0; i < res; ++i) {
                    buffer.push_back(temp[i]);
                }
            } while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() < timeout_ms_);
        }

    };

    auto SerialPort::available_ports() -> std::vector<SerialInfo> {
        return SerialPort::impl::available_ports();
    }

    SerialPort::SerialPort(std::string const &id, Baudrate baud, Parity par, Databits dbits, Stopbits sbits, unsigned timeout_ms) :
        pimpl_{std::make_unique<impl>(id, baud, par, dbits, sbits, timeout_ms)} {};
    
    SerialPort::~SerialPort()  = default;

    SerialPort::SerialPort(SerialPort &&rhs) = default;

    void SerialPort::set_params(Baudrate baud, Parity par, Databits dbits, Stopbits sbits, unsigned timeout_ms) {
        pimpl_->set_params(baud, par, dbits, sbits, timeout_ms);
    }

    auto SerialPort::write(std::vector<uint8_t> const& data) -> size_t {
        return pimpl_->write(data);
    }

    auto SerialPort::read() -> std::vector<uint8_t> {
        return pimpl_->read();
    }

    void SerialPort::read(std::vector<uint8_t> &buffer) {
        pimpl_->read(buffer);
    }

    void SerialPort::install_rx_listener(std::function<void(const std::vector<uint8_t>&)> func) {
        pimpl_->install_rx_listener(func);
    }

    void SerialPort::install_tx_listener(std::function<void(const std::vector<uint8_t>&)> func) {
        pimpl_->install_tx_listener(func);
    }

}
