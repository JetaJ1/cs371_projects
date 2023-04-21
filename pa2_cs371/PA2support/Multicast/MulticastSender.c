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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include "Practical.h"

int main(int argc, char *argv[]) {

  if (argc < 4 || argc > 5) // Test for number of parameters
    DieWithUserMessage("Parameter(s)", "<Multicast Address> <Port> <Send String> [<TTL>]");

  char *multicastIPString = argv[1];   // First arg:  multicast IP address
  char *service = argv[2];             // Second arg:  multicast port/service
  char *sendString = argv[3];          // Third arg:  string to multicast

  size_t sendStringLen = strlen(sendString);
  if (sendStringLen > MAXSTRINGLENGTH)        // Check input length
    DieWithUserMessage("String too long", sendString);

  // Fourth arg (optional):  TTL for transmitting multicast packets
  int multicastTTL = (argc == 5) ? atoi(argv[4]) : 1;

  // Tell the system what kind(s) of address info we want
  struct addrinfo addrCriteria;                   // Criteria for address match
  memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
  addrCriteria.ai_family = AF_INET;               // IPv4
  addrCriteria.ai_socktype = SOCK_DGRAM;          // Only datagram sockets
  addrCriteria.ai_protocol = /*[YOUR CODE HERE]*/ // Only UDP please
  addrCriteria.ai_flags |= AI_NUMERICHOST;        // Don't try to resolve address

  struct addrinfo *multicastAddr;   // Holder for returned address
  int rtnVal= getaddrinfo(multicastIPString, service, &addrCriteria, &multicastAddr);
  if (rtnVal != 0)
    DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));

  // Create socket for sending datagrams
  int sock = socket(multicastAddr->ai_family, multicastAddr->ai_socktype, multicastAddr->ai_protocol);
  if (sock < 0)
    DieWithSystemMessage("socket() failed");

  // Set TTL of multicast packet. 
  if (multicastAddr->ai_family == AF_INET) { // v4 specific
    // The v4 multicast TTL socket option requires that the value be
    // passed in an unsigned char
    u_char mcTTL = (u_char) multicastTTL;
    if (setsockopt(sock, IPPROTO_IP, /*[YOUR CODE HERE]*/, &mcTTL, sizeof(mcTTL)) < 0)
      DieWithSystemMessage("setsockopt() failed");
  } else {
    DieWithUserMessage("Unable to set TTL", "invalid address family");
  }

  for (;;) { // Run forever
    // Multicast the string to all who have joined the group (i.e., to multicastAddr)
    ssize_t numBytes = sendto(sock, sendString, sendStringLen, 0, /*[YOUR CODE HERE]*/, /*[YOUR CODE HERE]*/);
    if (numBytes < 0)
      DieWithSystemMessage("sendto() failed");
    else if (numBytes != sendStringLen)
      DieWithUserMessage("sendto()", "sent unexpected number of bytes");
    sleep(3);
  }
  // NOT REACHED
}