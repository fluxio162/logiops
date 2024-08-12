/*
 * Copyright 2019-2023 PixlOne
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef LOGID_BACKEND_RAW_IOMONITOR_H
#define LOGID_BACKEND_RAW_IOMONITOR_H

#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace logid::backend::raw {
    struct IOHandler {
        std::function<void()> read;
        std::function<void()> hangup;
        std::function<void()> error;

        IOHandler(std::function<void()> r,
                  std::function<void()> hup,
                  std::function<void()> err);
    };

    class IOMonitor {
    public:
        IOMonitor();

        IOMonitor(IOMonitor&&) = delete;

        IOMonitor(const IOMonitor&) = delete;

        IOMonitor& operator=(IOMonitor&&) = delete;

        IOMonitor& operator=(const IOMonitor&) = delete;

        ~IOMonitor() noexcept;

        void add(int fd, IOHandler handler);

        void remove(int fd) noexcept;
    private:
        void _listen(); // This is a blocking call
        void _stop() noexcept;
        std::unique_lock<std::mutex> _yield() noexcept;

        std::unique_ptr<std::thread> _io_thread;

        std::mutex _run_mutex;
        std::mutex _yield_mutex;

        std::map<int, std::shared_ptr<IOHandler>> _fds;
        std::atomic_bool _is_running;

        const int _epoll_fd;
        const int _event_fd;
    };
}

#endif //LOGID_BACKEND_RAW_IOMONITOR_H
