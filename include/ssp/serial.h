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

#ifndef SIMPLE_SERIAL_PORT_SERIAL_H
#define SIMPLE_SERIAL_PORT_SERIAL_H

#include <string>
#include <vector>
#include <memory>

namespace ssp {

    enum class baudrate : unsigned {
        _110 = 110,
        _300 = 300,
        _9600 = 9600
    };

    enum class parity {
        NONE, ODD, EVEN, MARK, SPACE
    };

    enum class databits {
        _5, _6, _7, _8
    };

    enum class stopbits {
        _1, _1POINT5, _2
    };

    enum class need_flush {
        NO, YES
    };

    struct serial_info {
        std::string id;
    };

    class serial_port {
    public:

        /**
         * Gets the list of available ports
         * @return vector with list of ports identifiers
         */
        static auto available_ports() -> std::vector<serial_info>;

        /**
         * Creates a new serial port
         * @param id : identifier of the serial port to be created (eg "COM1"in windows or "tty10"in linux)
         */
        serial_port(std::string const& id);
        ~serial_port();

        /**
         * Sets the serial port parameters
         * @param baud : baudrate in bits/sercond
         * @param par : parity type
         * @param dbits : number of data bits
         * @param sbits : number of stop bits
         */
        void set_params(baudrate baud, parity par, databits dbits, stopbits sbits);

        /**
         * Writes the contents of the data vector into the serial port
         * @param data : vector containing the data to be written
         * @return the number of bytes effectevely written
         */
        auto write(std::vector<uint8_t> const& data, need_flush f = need_flush::YES) -> size_t;

        /**
         * Flushes the output buffer
         */
        void flush();

        /**
         * Get the number of available bytes for reading
         * @return the number of available bytes
         */
        auto available() -> size_t;

        /**
         *
         * @param timeout_millis
         * @return
         */
        auto read(unsigned timeout_ms = 0) -> std::vector<uint8_t>;

        /**
         *
         * @param buffer
         * @param timeout_millis
         */
        void read(std::vector<uint8_t>& buffer, unsigned timeout_ms = 0);


    private:
        class impl;
        std::unique_ptr<impl> m_pimpl;
    };

    class open_serial_error : public std::exception {
    public:
        const char* what() const throw() {
            return "error while trying to open serial port";
        }
    };

    class serial_io_error : public std::exception {
    public:
        const char* what() const throw() {
            return "io error in serial port operation";
        }
    };

}

#endif //SIMPLE_SERIAL_PORT_SERIAL_H
