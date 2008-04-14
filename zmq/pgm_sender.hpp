/*
    Copyright (c) 2007-2008 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __PGM_SENDER_HPP_INCLUDED__
#define __PGM_SENDER_HPP_INCLUDED__

#include <glib.h>
#include <pgm/pgm.h>

#include <assert.h>

#include "stdint.hpp"

namespace zmq
{

    //  Encapsulates PGM socket
    class pgm_sender_t
    {
    public:
        // 
        pgm_sender_t (const char *network_, uint16_t port_);

        //  Closes the transport
        ~pgm_sender_t ();

        // 
        int get_fd (int *fds_, int nfds_);

        // 
        size_t write (unsigned char *data_, size_t size_);

        //
        size_t write_pkt (const struct iovec *iovec_, int niovec_);

    protected:
        pgm_transport_t* g_transport;
    };
}
#endif