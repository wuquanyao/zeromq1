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

#include <zmq/bp_decoder.hpp>
#include <zmq/wire.hpp>

zmq::bp_decoder_t::bp_decoder_t (i_demux *demux_) :
    demux (demux_)
{
    //  At the beginning, read one byte and go to one_byte_size_ready state.
    next_step (tmpbuf, 1, &bp_decoder_t::one_byte_size_ready);
}

void zmq::bp_decoder_t::reset ()
{
    //  Free the message buffer.
    message.rebuild (0);

    //  Restart the FSM.
    next_step (tmpbuf, 1, &bp_decoder_t::one_byte_size_ready);
}

bool zmq::bp_decoder_t::one_byte_size_ready ()
{
    //  First byte of size is read. If it is 0xff read 8-byte size.
    //  Otherwise allocate the buffer for message data and read the
    //  message data into it.
    if (*tmpbuf == 0xff)
        next_step (tmpbuf, 8, &bp_decoder_t::eight_byte_size_ready);
    else {
        message.rebuild (*tmpbuf);
        next_step (message.data (), *tmpbuf, &bp_decoder_t::message_ready);
    }
    return true;
}

bool zmq::bp_decoder_t::eight_byte_size_ready ()
{
    //  8-byte size is read. Allocate the buffer for message body and
    //  read the message data into it.
    message.rebuild ((size_t) get_uint64 (tmpbuf));
    next_step (message.data (), message.size (), &bp_decoder_t::message_ready);
    return true;
}

bool zmq::bp_decoder_t::message_ready ()
{
    //  Message is completely read. Push it to the dispatcher and start reading
    //  new message.
    if (!demux->write (message))
        return false;

    next_step (tmpbuf, 1, &bp_decoder_t::one_byte_size_ready);
    return true;
}

