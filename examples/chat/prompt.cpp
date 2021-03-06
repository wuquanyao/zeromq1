/*
    Copyright (c) 2007-2009 FastMQ Inc.

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
#include <zmq/platform.hpp>
#include <zmq/formatting.hpp>
#include <string.h>
#include <string>
#include <iostream>

using namespace std;

#include <zmq.hpp>

using namespace zmq;

int main (int argc, const char *argv [])
{
    //  Check the command line syntax.
    if (argc != 4) {
        cerr << "usage: prompt <host> <chatroom name> <user name>" << endl;
        return 1;
    }

    //  Retrieve command line arguments
    const char *host = argv [1];
    const char *chatroom_name = argv [2];
    const char *user_name = argv [3];

    //  Initialise 0MQ infrastructure.
    dispatcher_t dispatcher (2);
    locator_t locator (host);

    //  Initialise the thread layout
    i_thread *pt = io_thread_t::create (&dispatcher);
    api_thread_t *api = api_thread_t::create (&dispatcher, &locator);

    //  Create local exchange to send messages to.
    int eid = api->create_exchange ("E");

    //  Bind the exchange to globally accessible queue exposed by chatroom.
    //  If the queue is not available, assert.
    char tmp [256];
    zmq_snprintf (tmp, sizeof (tmp), "Q_%s", chatroom_name);
    api->bind ("E", tmp, pt, pt);
    
    while (true) {

        //  Allow user to input the message text. Prepend it by user name.
        char textbuf [1024];
        char *rcc = fgets (textbuf, sizeof (textbuf), stdin);
        assert (rcc);
        string text (user_name);
        text = text + ": " + textbuf;

        //  Create the message (terminating zero is part of the message)
        message_t message (text.size () + 1);
        memcpy (message.data (), text.c_str (), text.size () + 1);

        //  Send the message
        api->send (eid, message);
    }
}
