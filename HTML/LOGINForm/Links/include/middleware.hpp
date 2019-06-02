#pragma once
#ifndef __MIDDLEWARE__
#define __MIDDLEWARE__
#endif

#include <iostream>

#ifndef __APPLE__
#include <string.h>
#endif

#include <string>
#include <stdlib.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <vector>
#include<fstream>

#include <unistd.h>
#include <stdio.h>
#include <bits/stdc++.h> 

struct ClientInfo {
  struct sockaddr_in clientInfo;
  socklen_t size;
  int clientSocket;
};

/**
* This class is for middleware only
*/
class Middleware{

  // we must know on what port we are listening to
  int port;
  // we first have to define our server structures
  struct sockaddr_in server;

  // we have our sockets. Max 100 clients permitted.
  int server_socket;

  // addresses like 127.0.0.1:6061
  std::vector<std::string> knownAdresses;

public:
  Middleware();
  Middleware(int);
  ~Middleware();

  void startGlobalServer();
  void startLocalServer();
  static std::string readFromSocket(int&);
  static void writeToSocket(int, std::string);
  void setPort(int);
  struct ClientInfo waitClient();
  void getKnownAdresses(int );
  void askInfo();

};
