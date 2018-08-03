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

#ifndef SIMPLE_SerialPort_SERIAL_H
#define SIMPLE_SerialPort_SERIAL_H

#include <string>
#include <vector>
#include <memory>

namespace ssp
{

enum class Baudrate : unsigned
{
    _110 = 110,
    _300 = 300,
    _600 = 600,
    _1200 = 1200,
    _2400 = 2400,
    _4800 = 4800,
    _9600 = 9600,
    _14400 = 14400,
    _19200 = 19200,
    _38400 = 38400,
    _115200 = 115200,
};

enum class Parity
{
    NONE,
    ODD,
    EVEN,
    MARK,
    SPACE
};

enum class Databits
{
    _5,
    _6,
    _7,
    _8
};

enum class Stopbits
{
    _1,
    _1POINT5,
    _2
};

struct SerialInfo
{
    std::string id;
};

class SerialPort
{
public:

    /**
     * Gets the list of available ports
     * @return vector with list of ports identifiers
     */
    static auto available_ports() -> std::vector<SerialInfo>;

    /**
     * Creates a new serial port
     * @param id : identifier of the serial port to be created (eg "COM1"in windows or "tty10"in linux)
     */
    explicit SerialPort(std::string const &id,
                Baudrate baud = Baudrate::_9600,
                Parity par = Parity::NONE,
                Databits dbits = Databits::_8,
                Stopbits sbits = Stopbits::_1,
                unsigned timeout_ms = 5000);

    ~SerialPort();

    void set_baud(Baudrate baud);

    void set_parity(Parity par);

    void set_databits(Databits dbits);

    void set_stopbits(Stopbits sbits);

    void set_timeout(unsigned timeout_ms);

    /**
     * Sets the serial port parameters
     * @param baud : Baudrate in bits/sercond
     * @param par : Parity type
     * @param dbits : number of data bits
     * @param sbits : number of stop bits
     */
    void set_params(Baudrate baud, Parity par, Databits dbits, Stopbits sbits, unsigned timeout_ms);

    /**
     * Writes the contents of the data vector into the serial port
     * @param data : vector containing the data to be written
     * @return the number of bytes effectevely written
     */
    auto write(std::vector<uint8_t> const &data) -> size_t;

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
    auto read() -> std::vector<uint8_t>;

    /**
     *
     * @param buffer
     * @param timeout_millis
     */
    void read(std::vector<uint8_t> &buffer);

private:
    class impl;
    std::unique_ptr<impl> pimpl_;
};

class SerialErrorOpening : public std::exception
{
public:
    const char* what() const throw()
    {
        return "error opening serial port";
    }
};

class SerialErrorIO : public std::exception
{
public:
    const char* what() const throw()
    {
        return "IO error in serial port operation";
    }
};

class SerialErrorNotOpen : public std::exception
{
public:
    const char* what() const throw()
    {
        return "serial port is not open";
    }
};

class SerialErrorConfig : public std::exception
{
public:
    const char* what() const throw()
    {
        return "error while configuring serial port";
    }
};

}

#endif //SIMPLE_SerialPort_SERIAL_H
