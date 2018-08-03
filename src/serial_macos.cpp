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

namespace ssp
{
    struct SerialPort::impl {

        static auto available_ports() -> std::vector<SerialInfo> {
            std::vector<SerialInfo> retval;
            //TODO
            return retval;
        }

        void set_params(Baudrate baud, Parity par, Databits dbits, Stopbits sbits) {
            //TODO
        }

        auto write(std::vector<uint8_t> const &data, need_flush f) -> size_t {
            //TODO
            if (f == need_flush::YES) {
                flush();
            }
            return 0;
        }

        void flush() {
            //TODO
        }

        auto read(unsigned timeout_ms) -> std::vector<uint8_t> {
            std::vector<uint8_t> retval;
            read(retval, timeout_ms);
            return retval;
        }

        void read(std::vector<uint8_t> &buffer, unsigned timeout_ms) {
            //TODO
        }

    };

    auto SerialPort::available_ports() -> std::vector<SerialInfo> {
        return SerialPort::impl::available_ports();
    }

    SerialPort::SerialPort(std::string const& id) : m_pimpl{std::make_unique<SerialPort::impl>(id)} {}
    SerialPort::~SerialPort()  = default;

    void SerialPort::set_params(Baudrate baud, Parity par, Databits dbits, Stopbits sbits) {
        m_pimpl->set_params(baud, par, dbits, sbits);
    }

    auto SerialPort::write(std::vector<uint8_t> const &data, need_flush f) -> size_t {
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
