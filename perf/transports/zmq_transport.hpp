/*
    Copyright (c) 2007-2009 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify it under
    the terms of the Lesser GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    Lesser GNU General Public License for more details.

    You should have received a copy of the Lesser GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __PERF_ZMQ_TRANSPORT_HPP_INCLUDED__
#define __PERF_ZMQ_TRANSPORT_HPP_INCLUDED__

#include "i_transport.hpp"

#include <zmq.hpp>

namespace perf
{

    class zmq_t : public i_transport
    {
    public:
        zmq_t (const char *host_, bool bind_, const char *exchange_name_,
              const char *queue_name_, const char *exchange_interface_,
              const char *queue_interface_) :
            dispatcher (2),
            locator (host_)
        {

            api = zmq::api_thread_t::create (&dispatcher, &locator);
            worker = zmq::io_thread_t::create (&dispatcher);

            if (bind_) {

                assert (!exchange_interface_);
                assert (!queue_interface_);

                //  Create & bind local exchange.
                exchange_id = api->create_exchange ("E_LOCAL",
                    zmq::scope_local, NULL, NULL, 0, NULL,
                    zmq::style_load_balancing);
                api->bind ("E_LOCAL", queue_name_, worker, worker);
                
                //  Create & bind local queue.
                api->create_queue ("Q_LOCAL");
                api->bind (exchange_name_, "Q_LOCAL", worker, worker);

            } else {
                assert (exchange_interface_);
                assert (queue_interface_);
                
                api->create_queue (queue_name_, zmq::scope_global,
                    queue_interface_, worker, 1, &worker);

                exchange_id = api->create_exchange (exchange_name_, 
                    zmq::scope_global, exchange_interface_, worker, 
                    1, &worker, zmq::style_load_balancing);
            }
          
        }

        inline ~zmq_t ()
        {
#ifdef ZMQ_HAVE_WINDOWS
	    Sleep (1000);
#else
            sleep (1);
#endif
        }

        inline virtual void send (size_t size_)
        {
            zmq::message_t message (size_);
            api->send (exchange_id, message);
        }

        inline virtual size_t receive ()
        {
            zmq::message_t message;
            api->receive (&message);
            return message.size ();
        }

    protected:

        zmq::dispatcher_t dispatcher;
        zmq::locator_t locator;
        zmq::api_thread_t *api;
        zmq::i_thread *worker;
	int exchange_id;
    };

}

#endif
