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
    struct serial_port::impl {

        static auto available_ports() -> std::vector<serial_info> {
            std::vector<serial_info> retval;
            //TODO
            return retval;
        }

        void set_params(baudrate baud, parity par, databits dbits, stopbits sbits) {
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

    auto serial_port::available_ports() -> std::vector<serial_info> {
        return serial_port::impl::available_ports();
    }

    serial_port::serial_port(std::string const& id) : m_pimpl{std::make_unique<serial_port::impl>(id)} {}
    serial_port::~serial_port()  = default;

    void serial_port::set_params(baudrate baud, parity par, databits dbits, stopbits sbits) {
        m_pimpl->set_params(baud, par, dbits, sbits);
    }

    auto serial_port::write(std::vector<uint8_t> const &data, need_flush f) -> size_t {
        return m_pimpl->write(data, f);
    }

    void serial_port::flush() {
        m_pimpl->flush();
    }

    auto serial_port::read(unsigned int timeout_ms) -> std::vector<uint8_t> {
        return m_pimpl->read(timeout_ms);
    }

    void serial_port::read(std::vector<uint8_t> &buffer, unsigned int timeout_ms) {
        m_pimpl->read(buffer, timeout_ms);
    }

}