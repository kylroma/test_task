#include "server.h"
#include "processRequests.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <err.h>
#include <errno.h>
#include <string.h>

#include <vector>


#include <iostream>
#include <unistd.h>
using namespace std;

Server::Server() : isRunServer(true)
{

}

void Server::runServer()
{
    if(connect()) {
        mListenThread = std::thread(&Server::listenConnections, this);
    }
}

Server::~Server()
{
    if(mListenThread.joinable()) {
        mListenThread.join();
    }
}

void Server::stopServer()
{
    isRunServer = false;
    shutdown(mSocet, SHUT_RDWR);
}

bool Server::connect()
{
    bool result = false;
    int one = 1;
    sockaddr_in svrAddr = {};

    mSocet = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocet < 0) {
       cerr << "Can't open socket: " <<  strerror(errno) << endl;
       return result;
    }

    setsockopt(mSocet, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));

    int port = 8080;
    svrAddr.sin_family = AF_INET;
    svrAddr.sin_addr.s_addr = INADDR_ANY;
    svrAddr.sin_port = htons(port);

    if (bind(mSocet, (struct sockaddr *) &svrAddr, sizeof(svrAddr)) == -1) {
      close(mSocet);
      cerr << "Can't bind: " <<  strerror(errno) << endl;
      return result;
    }

    return true;
}

void Server::listenConnections()
{
    sockaddr_in cliAddr = {};
    socklen_t sinLen = sizeof(cliAddr);
    const short BUFFER_SIZE = 1024;
    vector<char> buffer(BUFFER_SIZE);
    ssize_t sizeRead = 0;

    listen(mSocet, 5);

    while (isRunServer.load()) {
      int clientFd = accept(mSocet, (struct sockaddr *) &cliAddr, &sinLen);

      if (clientFd == -1 ) {
          if(isRunServer.load()) {
              cerr << "Can't accept: " <<  strerror(errno) << endl;
          }
          continue;
      }

      sizeRead = read( clientFd , buffer.data(), buffer.size()-1);

      if(sizeRead < 1) {
          continue;
      }

      buffer[sizeRead] = '\0';

      auto processReqest = [](int clientSocket, string request){
          ProcessRequests pRequest;
          string response = pRequest.processRequest(request);
          cout << response << endl << endl;

          for(unsigned total = 0; total < response.size(); ) {
              int num = send(clientSocket, (response.c_str() + total)
                          , (response.size() - total), 0);

              if(num == -1) {
                  break;
              }
              total += num;
          }

          close(clientSocket);
      };

      string request = buffer.data();
      thread(processReqest, clientFd, request).detach();
    }
}
