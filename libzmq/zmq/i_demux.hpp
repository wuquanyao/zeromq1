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

#ifndef __ZMQ_I_DEMUX_HPP_INCLUDED__
#define __ZMQ_I_DEMUX_HPP_INCLUDED__

#include <zmq/i_destination.hpp>
#include <zmq/pipe.hpp>
#include <zmq/i_producer.hpp>

namespace zmq
{

    //  Interface to be implemented by message distributors.
    //  Each message distributor distributes incomming
    //  messages among its pipes. Also, a message can be sent
    //  to more then one pipe.

    class i_demux : public i_destination
    {
    public:

        virtual ~i_demux () {};

        //  Returns high and low watermarks for a specified demux. These are
        //  used to derive high and low watermarks for pipes attached to
        //  the demux. When hwm is zero, the pipe will be of infinite capacity.
        virtual void get_watermarks (int64_t &hwm_, int64_t &lwm_) = 0;

        //  Associate the specified engine with the demux.
        virtual void register_producer (i_producer *producer_) = 0;

        //  Starts sending messages to this pipe.
        virtual void send_to (pipe_t *pipe_) = 0;

        //  Flushes all messages.
        virtual void flush () = 0;

        //  Inform the demux that the pipe can accept more messages now.
        virtual void pipe_ready (pipe_t *pipe_) = 0;

        //  Writes the gap notification to all attached pipes.
        virtual void gap () = 0;

        //  Returns true if no pipe is attached to this message distributor.
        virtual bool empty () = 0;

        //  Stops sending messages to this pipe.
        virtual void release_pipe (pipe_t *pipe_) = 0;

        //  Initiates shutdown of all attached pipes.
        virtual void initialise_shutdown () = 0;

        //  Returns true if there is no pipe we can send message to.
        inline bool no_pipes ()
        {
            return empty ();
        }

    };

}

#endif