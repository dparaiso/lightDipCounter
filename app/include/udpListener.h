// udpListener.h
// Module to listen to port PORT in the background (uses a thread).
//
// The application will continuously check for messages received and give
// the correct output. 

// The following are the accepted commands:
// help    -- Return a brief summary/list of supported commands.
// ?       -- same as help
// count   -- get the total number of samples taken.
// length  -- get the number of samples taken in the previously completed
// second.
// dips    -- get the number of dips in the previously completed second.
// history -- get all the samples in the previously completed second.
// stop    -- cause the server program to end.
// <enter> -- repeat last command


#ifndef _UDPLISTENER_H_
#define _UDPLISTENER_H_

#define PORT 12345
#define BUFFER_SIZE 16384

// Begins the background thread which listens to UDP PORT.
// Thread finishes when stop is called
void UDP_init();

// Listens to PORT for messages
void* UDP_startListening();

// Takes the received message in buff and populates msg with the correct response
void UDP_parseMessage(char* buff, int bytesRead, char* msg);

#endif