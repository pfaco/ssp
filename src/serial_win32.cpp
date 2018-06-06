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
#include <chrono>

namespace ssp
{
    class serial_port::impl
    {
        HANDLE hserial;
        bool is_open;

    public:
        static auto available_ports() -> std::vector<serial_info>
        {
            std::vector<serial_info> retval;

            for (unsigned i = 0; i <= 255; ++i) {

            }

            return retval;
        }

        impl(std::string const& id,
             baudrate baud,
             parity par,
             databits dbits,
             stopbits sbits)
        {
            std::string long_prefix = "\\\\.\\";
            std::string prefix = "COM";
            std::string port_name = id;
            if (id.compare(0, prefix.size(), prefix) && id.size() > 4) {
                port_name.insert(0, long_prefix);
            }
            hserial = CreateFile(port_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
            if (hserial == INVALID_HANDLE_VALUE) {
                throw open_serial_error{};
            }
            set_params(baud, par, dbits ,sbits);
        }

        ~impl()
        {
            CloseHandle(hserial); //Closing the Serial Port
        }

        void set_baud(baudrate baud) {
            DCB dcb_params = { 0 };
            dcb_params.DCBlength = sizeof(dcb_params);
            auto status = GetCommState(hserial, &dcb_params);
            dcb_params.BaudRate = static_cast<unsigned>(baud);
            SetCommState(hserial, &dcb_params);
        }

        void set_parity(parity par) {
            DCB dcb_params = { 0 };
            dcb_params.DCBlength = sizeof(dcb_params);
            auto status = GetCommState(hserial, &dcb_params);
            switch (par) {
                case parity::EVEN:
                    dcb_params.Parity   = EVENPARITY;
                    break;
                case parity::ODD:
                    dcb_params.Parity   = ODDPARITY;
                    break;
                case parity::NONE:
                    dcb_params.Parity   = NOPARITY;
                    break;
                case parity::MARK:
                    dcb_params.Parity   = MARKPARITY;
                    break;
                case parity::SPACE:
                    dcb_params.Parity   = SPACEPARITY;
                    break;
            }
            SetCommState(hserial, &dcb_params);
        }

        void set_databits(databits dbits) {
            DCB dcb_params = { 0 };
            dcb_params.DCBlength = sizeof(dcb_params);
            auto status = GetCommState(hserial, &dcb_params);
            switch (dbits) {
                case databits::_5:
                    dcb_params.ByteSize = 5;
                    break;
                case databits::_6:
                    dcb_params.ByteSize = 6;
                    break;
                case databits::_7:
                    dcb_params.ByteSize = 7;
                    break;
                case databits::_8:
                    dcb_params.ByteSize = 8;
                    break;
            }
            SetCommState(hserial, &dcb_params);
        }

        void set_stopbits(stopbits sbits) {
            DCB dcb_params = { 0 };
            dcb_params.DCBlength = sizeof(dcb_params);
            auto status = GetCommState(hserial, &dcb_params);
            switch (sbits) {
                case stopbits::_1:
                    dcb_params.StopBits = ONESTOPBIT;// Setting StopBits = 1
                    break;
                case stopbits::_1POINT5:
                    dcb_params.StopBits = ONE5STOPBITS;// Setting StopBits = 1
                    break;
                case stopbits::_2:
                    dcb_params.StopBits = TWOSTOPBITS;// Setting StopBits = 1
                    break;
            }
            SetCommState(hserial, &dcb_params);
        }

        void set_params(baudrate baud, parity par, databits dbits, stopbits sbits)
        {
            set_baud(baud);
            set_parity(par);
            set_databits(dbits);
            set_stopbits(sbits);
        }

        auto write(std::vector<uint8_t> const& data, need_flush f) -> size_t
        {
            DWORD n_bytes_writen = 0;
            auto status = WriteFile(hserial, &data[0], data.size(), &n_bytes_writen, NULL);
            if (f == need_flush::YES) {
                flush();
            }
            return n_bytes_writen;
        }

        void flush()
        {

        }

        auto available() -> size_t
        {

        }

        auto read(unsigned timeout_ms) -> std::vector<uint8_t>
        {
            std::vector<uint8_t> retval;
            read(retval, timeout_ms);
            return retval;
        }

        void read(std::vector<uint8_t> &buffer, unsigned timeout_ms)
        {
            auto start = std::chrono::steady_clock::now();
            auto duration = std::chrono::steady_clock::now() - start;
            uint8_t temp[128];
            DWORD amount_read;
            do {
                ReadFile(hserial, temp, sizeof(temp), &amount_read, NULL);
                for (auto i = 0; i < amount_read; ++i) {
                    buffer.push_back(temp[i]);
                }
                duration = std::chrono::steady_clock::now() - start;
            } while (amount_read == 0 && std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() < timeout_ms);
        }

    };

    auto serial_port::available_ports() -> std::vector<serial_info>
    {
        return impl::available_ports();
    }

    serial_port::serial_port(std::string const& id,
                             baudrate baud,
                             parity par,
                             databits dbits,
                             stopbits sbits) : m_pimpl{std::make_unique<impl>(id, baud, par, dbits, sbits)} {}

    serial_port::~serial_port() = default;

    void serial_port::set_params(baudrate baud, parity par, databits dbits, stopbits sbits)
    {
        m_pimpl->set_params(baud, par, dbits, sbits);
    }

    auto serial_port::write(std::vector<uint8_t> const& data, need_flush f) -> size_t
    {
        return m_pimpl->write(data, f);
    }

    void serial_port::flush()
    {
        m_pimpl->flush();
    }

    auto serial_port::available() -> size_t
    {
        return m_pimpl->available();
    }

    auto serial_port::read(unsigned timeout_ms) -> std::vector<uint8_t>
    {
        return m_pimpl->read(timeout_ms);
    }

    void serial_port::read(std::vector<uint8_t> &buffer, unsigned timeout_ms)
    {
        m_pimpl->read(buffer, timeout_ms);
    }

}