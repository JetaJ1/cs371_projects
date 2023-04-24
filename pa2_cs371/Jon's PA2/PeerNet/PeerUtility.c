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
 *
 * Adapted by Yang Xiao (xiaoy@uky.edu) for instructional purposes in 
 * CS371 at the University of Kentucky
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include "Practical.h"

/*
 * [YOUR CODE HERE]: Please modify this function to fulfill the design goals.
 *
 * The listening peer needs to handle three types of TCP connections:
 *
 * -- Type 1: A new joining peer without a message (-s option not used).
 *    You only need to send back a signal indicating the join is successful or
 *    not, along with the IP address and listening port of your first peer if
 *    your peer list is not empty. If the join is successful, add this new guy
 *    to your peer list.
 *
 * -- Type 2: A new joining peer with a message (-s option used).
 *    Besides the above response and peer list operation, you also need to propagate
 *    the message to your peers if your peer list is not empty.
 *
 * -- Type 3: An existing peer with a message.
 *    If the TCP client is an existing peer of yours, you need to propagate the
 *    message to your other peers if there is any.
*/
void PropagateMessage(int clientSocket, int peerSocket, char *message, ssize_t numBytesRcvd, int hops) {
  char hopsAsStr[BUFSIZE];
  sprintf(hopsAsStr, "%d", hops + 1);

  // Send received string and receive again until end of stream
  while (numBytesRcvd > 0) { // 0 indicates end of stream
    Send(peerSocket, message, numBytesRcvd, 0, "propagatedMessage");

    Recv(peerSocket, ack, BUFSIZE, 0, "message ack");
    Send(peerSocket, hopsAsStr, strlen(hopsAsStr), 0, "hops");

    // See if there is more data to receive
    numBytesRcvd = recv(clientSocket, message, BUFSIZE, MSG_DONTWAIT);
    if (numBytesRcvd < 0) {
      if (DEBUG)
        puts("message finished propagating");
      return;
    }
  }

  if (DEBUG)
    puts("message finished propagating");
}


int SetupTCPClientSocket(const char *serverIP, const char *serverPort, const char *this_portAsStr) {
  // Create socket for outgoing connection
  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock < 0)
    DieWithSystemMessage("socket() failed");

  // Construct the server address structure
  struct sockaddr_in servAddr;            // Server address
  memset(&servAddr, 0, sizeof(servAddr)); // Zero out structure
  servAddr.sin_family = AF_INET;          // IPv4 address family
  // Convert address
  int rtnVal = inet_pton(AF_INET, serverIP, &servAddr.sin_addr.s_addr);
  if (rtnVal == 0)
    DieWithUserMessage("inet_pton() failed", "invalid address string");
  else if (rtnVal < 0)
    DieWithSystemMessage("inet_pton() failed");
  servAddr.sin_port = htons(atoi(serverPort));    // Server port

  // Establish the connection to the peer
  if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
    DieWithSystemMessage("connect() failed");

  // Tells the server which port we're running on
  Send(sock, this_portAsStr, strlen(this_portAsStr), 0, "this_portAsStr");
  Recv(sock, ack, BUFSIZE, 0, "this_portAsStr ack");

  return sock;
}


// Wrapper and error handler for socket send()
ssize_t Send(int sockfd, const char *buf, size_t len, int flags, char *infoType) {
  if (DEBUG)
    printf("sending %s...\n", infoType);

  ssize_t numBytes = send(sockfd, buf, len, flags);

  if (DEBUG)
    printf("sent: %s, length %ld\n", buf, len);

  if (numBytes < 0)
    DieWithSystemMessage("send() failed");
  else if (numBytes != len)
    DieWithUserMessage("send()", "sent unexpected number of bytes");

  return numBytes;
}


// Wrapper and error handler for socket recv()
ssize_t Recv(int sockfd, char *buf, size_t len, int flags, char *infoType) {
  if (DEBUG)
    printf("waiting to receive %s...\n", infoType);

  ssize_t numBytes = recv(sockfd, buf, len - 1, flags);
  buf[numBytes] = '\0';

  if (DEBUG)
    printf("received: %s, length %ld\n", buf, len);

  if (numBytes < 0)
    DieWithSystemMessage("recv() failed");
  else if (numBytes == 0)
    DieWithUserMessage("recv()", "connection closed prematurely");

  return numBytes;
}