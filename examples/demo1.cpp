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
        ssp::serial_port serial("/dev/pts/3");
        serial.set_params(ssp::baudrate::_9600, ssp::parity::NONE, ssp::databits::_8, ssp::stopbits::_1);
        serial.write({'P', 'a', 'u', 'l', 'o', 0x7E});
        std::vector<uint8_t> data;
        while (data.size() == 0) {
            data = serial.read();
        }
        std::cout << "RX: ";
        for (auto const& b : data) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b) << ' ';
        }
    } catch (ssp::open_serial_error const& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
