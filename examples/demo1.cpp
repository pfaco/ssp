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

#include <ssp/serial.h>
#include <iostream>
#include <iomanip>

auto main() -> int {

    try {
        //ssp::serial_port serial("/dev/pts/3");
        ssp::serial_port serial("COM8", ssp::baudrate::_300, ssp::parity::EVEN, ssp::databits::_7, ssp::stopbits::_1);
        serial.write({0x2f, 0x3f, 0x21, 0x0d, 0x0a});
        auto data = serial.read();
        std::cout << "RX: ";
        for (auto const& b : data) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b) << ' ';
        }
    } catch (ssp::open_serial_error const& e) {
        std::cout << e.what() << std::endl;
    } catch (ssp::serial_io_error const& e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "unexpected exception" << std::endl;
    }

    return 0;
}
