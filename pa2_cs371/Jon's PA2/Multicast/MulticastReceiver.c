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

  if (argc != 3)
    DieWithUserMessage("Parameter(s)", "<Multicast Address> <Port>");

  char *multicastAddrString = argv[1]; // First arg: multicast addr (v4 or v6!)
  char *service = argv[2];             // Second arg: port/service

  struct addrinfo addrCriteria;                   // Criteria for address match
  memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
  addrCriteria.ai_family = AF_INET;               // IPv4
  addrCriteria.ai_socktype = SOCK_DGRAM;          // Only datagram sockets
  addrCriteria.ai_protocol = IPPROTO_UDP;         // Only UDP protocol
  addrCriteria.ai_flags |= AI_NUMERICHOST;        // Don't try to resolve address

  // Get address information
  struct addrinfo *multicastAddr;                 // List of server addresses
  int rtnVal = getaddrinfo(multicastAddrString, service, &addrCriteria, &multicastAddr);
  if (rtnVal != 0)
    DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));

  // Create socket to receive on
  int sock = socket(multicastAddr->ai_family, multicastAddr->ai_socktype, multicastAddr->ai_protocol);
  if (sock < 0)
    DieWithSystemMessage("socket() failed");

  if (bind(sock, multicastAddr->ai_addr, multicastAddr->ai_addrlen) < 0)
    DieWithSystemMessage("bind() failed");

  // Unfortunately we need some address-family-specific pieces
  if (multicastAddr->ai_family == AF_INET) {
    // Now join the multicast "group"
    struct ip_mreq joinRequest;
    joinRequest.imr_multiaddr = ((struct sockaddr_in *) multicastAddr->ai_addr)->sin_addr;
    joinRequest.imr_interface.s_addr = 0;  // Let the system choose the i/f
    printf("Joining IPv4 multicast group...\n");
    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &joinRequest, sizeof(joinRequest)) < 0)
      DieWithSystemMessage("setsockopt() failed");
  } else {
    DieWithSystemMessage("Unknown address family");
  }
  // Free address structure(s) allocated by getaddrinfo()
  freeaddrinfo(multicastAddr);

  for (;;) { // Run forever
    char recvString[MAXSTRINGLENGTH + 1]; // Buffer to receive into
    // Receive a single datagram from the server
    int recvStringLen = recvfrom(sock, recvString, MAXSTRINGLENGTH, 0, NULL, 0);
    if (recvStringLen < 0)
      DieWithSystemMessage("recvfrom() failed");

    recvString[recvStringLen] = '\0';    // Terminate the received string
    // Note: sender did not send the terminal 0
    printf("Received: %s\n", recvString);
  }
  
  close(sock);
  exit(0);
}