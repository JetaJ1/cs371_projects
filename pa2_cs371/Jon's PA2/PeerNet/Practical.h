/* 
 * Copyright (c) 2009 Morgan Kaufmann.
 * Authors: Michael J. Donahoo and Kenneth L. Calvert
 * All rights reserved.
 *
 * The authors and the Publisher DISCLAIM ALL EXPRESS AND IMPLIED 
 * WARRANTIES, including warranties of merchantability and fitness 
 * for any particular purpose. Your use or reliance upon any sample 
 * code or other information in this book will be at your own risk. 
 * No one should use any sample code (or illustrations) from this 
 * book in any software application without first obtaining competent 
 * legal advice.
 */

#ifndef PRACTICAL_H_
#define PRACTICAL_H_

#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>

static const bool DEBUG = false;

enum sizeConstants {
  MAXSTRINGLENGTH = 128,
  BUFSIZE = 512,
  MAXPEERS = 2
};

struct peerInfo {
  int sock;
  char IP[BUFSIZE];
  char port[BUFSIZE];
};

// Handle error with user msg
void DieWithUserMessage(const char *msg, const char *detail);
// Handle error with sys msg
void DieWithSystemMessage(const char *msg);
// Handle new TCP client
void PropagateMessage(int clientSocket, int peerSocket, char *message, ssize_t numBytesRcvd, int hops);
// Setup new socket that will be used to connect to a remote server
int SetupTCPClientSocket(const char *serverIP, const char *serverPort, const char *this_portAsStr);
// Wrapper for socket functions
ssize_t Send(int sockfd, const char *buf, size_t len, int flags, char *infoType);
ssize_t Recv(int sockfd, char *buf, size_t len, int flags, char *infoType);

// Storage for returned acks
static char ack[BUFSIZE];

#endif // PRACTICAL_H_
