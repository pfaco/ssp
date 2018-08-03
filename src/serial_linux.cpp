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

        impl(std::string const &id,
             Baudrate baud,
             Parity par,
             Databits dbits,
             Stopbits sbits)
        {
            if ((fd = open(id.c_str(), O_RDWR | O_NOCTTY)) < 0) {
                throw SerialErrorOpening();
            }
            set_params(Baudrate::_9600, Parity::NONE, Databits::_8, Stopbits::_1);
            flush();
        }

        ~impl()
        {
            close(fd);
        }

        static auto available_ports() -> std::vector<SerialInfo>
        {
            std::vector<SerialInfo> retval;
            return retval;
        }

        void set_params(Baudrate baud, Parity par, Databits dbits, Stopbits sbits)
        {
            struct termios params;
            memset(&params, 0, sizeof(params));

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

        auto write(std::vector<uint8_t> const &data, NeedFlush f) -> size_t
        {
            ::write(fd, data.data(), data.size());
            if (f == NeedFlush::YES) {
                flush();
            }
            return 0;
        }

        void flush()
        {
            tcflush(fd, TCIFLUSH);
        }

        auto read(unsigned timeout_ms) -> std::vector<uint8_t>
        {
            std::vector<uint8_t> retval;
            read(retval, timeout_ms);
            return retval;
        }

        void read(std::vector<uint8_t> &buffer, unsigned timeout_ms)
        {
            uint8_t temp[128];
            auto start = std::chrono::steady_clock::now();
            do {
                auto res = ::read(fd, temp, sizeof(temp));
                for (auto i = 0; i < res; ++i) {
                    buffer.push_back(temp[i]);
                }
            } while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() < timeout_ms);
        }

    };

    auto SerialPort::available_ports() -> std::vector<SerialInfo> {
        return SerialPort::impl::available_ports();
    }

    SerialPort::SerialPort(std::string const &id, Baudrate baud, Parity par, Databits dbits, Stopbits sbits) :
        m_pimpl{std::make_unique<impl>(id, baud, par, dbits, sbits)} {};
    SerialPort::~SerialPort()  = default;

    void SerialPort::set_params(Baudrate baud, Parity par, Databits dbits, Stopbits sbits) {
        m_pimpl->set_params(baud, par, dbits, sbits);
    }

    auto SerialPort::write(std::vector<uint8_t> const& data, NeedFlush f) -> size_t {
        return m_pimpl->write(data, f);
    }

    void SerialPort::flush() {
        m_pimpl->flush();
    }

    auto SerialPort::read(unsigned int timeout_ms) -> std::vector<uint8_t> {
        return m_pimpl->read(timeout_ms);
    }

    void SerialPort::read(std::vector<uint8_t> &buffer, unsigned int timeout_ms) {
        m_pimpl->read(buffer, timeout_ms);
    }

}
