/*
 * Copyright (C) 2015-2020 Kurt Kanzenbach <kurt@kmk-computers.de>
 *
 * This file is part of Get.
 *
 * Get is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Get is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Get.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "tcp_connection.h"

#include "logger.h"
#include "progress_bar.h"

#include <cstring>
#include <stdexcept>
#include <algorithm>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

void TCPConnection::connect(const std::string& host, int port)
{
    connect(host, std::to_string(port));
}

void TCPConnection::connect(const std::string& host, const std::string& service)
{
    close();
    tcp_connect(host, service);
    set_default_timeout();
    m_connected = true;
}

void TCPConnection::write(const std::string& to_write) const
{
    ssize_t written = 0;
    auto len = to_write.size();
    auto start = to_write.c_str();

    if (!m_connected)
        EXCEPTION("Not connected!");

    while (static_cast<decltype(len)>(written) < len) {
        auto tmp = ::write(m_sock, start + written, len - written);
        if (tmp == -1)
            EXCEPTION("write() to socket failed: ", strerror(errno));
        written += tmp;
    }
}

std::string TCPConnection::read(std::size_t num_bytes) const
{
    std::string result;
    char buffer[BUFFER_SIZE];
    ssize_t read = 0;

    if (!m_connected)
        EXCEPTION("Not connected!");

    while (static_cast<decltype(num_bytes)>(read) < num_bytes) {
        auto len = std::min(sizeof(buffer), num_bytes - static_cast<std::size_t>(read));
        auto tmp = ::read(m_sock, buffer, len);
        if (tmp < 0)
            EXCEPTION("read() to socket failed: ", strerror(errno));
        if (tmp == 0)
            EXCEPTION("read() encountered EOF");
        result.insert(read, buffer, tmp);
        read += tmp;
    }

    return result;
}

std::string TCPConnection::read_until_eof(std::size_t file_size) const
{
    std::string result;
    char buffer[BUFFER_SIZE];
    ssize_t read = 0;

    if (!m_connected)
        EXCEPTION("Not connected!");

    if (file_size > 0)
        result.reserve(file_size);
    while (42) {
        auto tmp = ::read(m_sock, buffer, sizeof(buffer));
        if (tmp == -1)
            EXCEPTION("read() to socket failed: ", strerror(errno));
        if (tmp == 0)
            break;
        result.insert(read, buffer, tmp);
        read += tmp;
    }

    return result;
}

std::string TCPConnection::read_until_eof_with_pg(std::size_t file_size) const
{
    ProgressBar pg(file_size);
    std::string result;
    char buffer[BUFFER_SIZE];
    ssize_t read = 0;

    if (!m_connected)
        EXCEPTION("Not connected!");

    if (file_size > 0)
        result.reserve(file_size);
    while (42) {
        auto tmp = ::read(m_sock, buffer, sizeof(buffer));
        if (tmp == -1)
            EXCEPTION("read() to socket failed: ", strerror(errno));
        if (tmp == 0)
            break;
        result.insert(read, buffer, tmp);
        read += tmp;
        pg.update(tmp);
    }

    return result;
}

void TCPConnection::read_until_eof_to_fstream(std::ofstream& ofs) const
{
    char buffer[BUFFER_SIZE];

    if (!m_connected)
        EXCEPTION("Not connected!");

    while (42) {
        auto tmp = ::read(m_sock, buffer, sizeof(buffer));
        if (tmp == -1)
            EXCEPTION("read() to socket failed: ", strerror(errno));
        if (tmp == 0)
            break;
        ofs.write(buffer, tmp);
    }
}

void TCPConnection::read_until_eof_with_pg_to_fstream(std::ofstream& ofs, std::size_t start_offset, std::size_t file_size) const
{
    ProgressBar pg(start_offset, file_size);
    char buffer[BUFFER_SIZE];

    if (!m_connected)
        EXCEPTION("Not connected!");

    while (42) {
        auto tmp = ::read(m_sock, buffer, sizeof(buffer));
        if (tmp == -1)
            EXCEPTION("read() to socket failed: ", strerror(errno));
        if (tmp == 0)
            break;
        ofs.write(buffer, tmp);
        pg.update(tmp);
    }
}

std::string TCPConnection::read_ln() const
{
    std::string result;
    char buffer[1];
    ssize_t read = 0;

    if (!m_connected)
        EXCEPTION("Not connected!");

    // FIXME: This is kind of slow...
    while (42) {
        auto tmp = ::read(m_sock, buffer, 1);
        if (tmp < 0)
            EXCEPTION("read() to socket failed: ", strerror(errno));
        if (tmp == 0)
            EXCEPTION("read() in read_ln() encountered EOF");
        result.insert(read, buffer, tmp);
        read += tmp;

        if (buffer[0] == '\n')
            break;
    }

    return result;
}
