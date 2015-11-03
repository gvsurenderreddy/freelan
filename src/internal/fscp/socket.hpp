/*
 * freelan - An open, multi-platform software to establish peer-to-peer virtual
 * private networks.
 *
 * Copyright (C) 2010-2011 Julien KAUFFMANN <julien.kauffmann@freelan.org>
 *
 * This file is part of freelan.
 *
 * freelan is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * freelan is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 *
 * If you intend to use freelan in a commercial software, please
 * contact me : we may arrange this for a small fee or no fee at all,
 * depending on the nature of your project.
 */

/**
 * \file socket.hpp
 * \author Julien KAUFFMANN <julien.kauffmann@freelan.org>
 * \brief A FSCP socket.
 */

#pragma once

#include <map>
#include <memory>

#include <boost/asio.hpp>

#include "message.hpp"
#include "endpoint_context.hpp"

namespace freelan {
    class Socket {
        public:
            typedef boost::asio::ip::udp::socket::endpoint_type Endpoint;

            Socket(boost::asio::io_service& io_service) :
                m_socket(io_service)
            {}

            template<typename WriteHandler>
            void async_greet(const Endpoint& destination, WriteHandler handler) {
                const auto& endpoint_context = get_endpoint_context_for(destination);
                const auto unique_number = endpoint_context.get_next_hello_request_number();
                const size_t required_size = write_fscp_hello_request_message(nullptr, 0, unique_number);

                assert(required_size != 0);

                const auto buf = std::make_shared<std::vector<char>>(required_size);
                m_socket.async_send_to(&(*buf)[0], destination, [buf, handler](const boost::system::error_code& ec, std::size_t bytes_transferred) {
                    handler(ec, bytes_transferred);
                });
            }

            EndpointContext& get_endpoint_context_for(const Endpoint& endpoint) {
                return m_endpoint_contexts[endpoint];
            }

        private:
            boost::asio::ip::udp::socket m_socket;
            std::map<Endpoint, EndpointContext> m_endpoint_contexts;
    };
}
