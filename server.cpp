#include "server.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include <vector>
#include <iostream>
#include <unistd.h>

using namespace std;

Server::Server() : isRunServer(true)
{

}

bool Server::runServer()
{
    isRunServer = connect();
    if(isRunServer.load()) {
        mListenThread = std::thread(&Server::listenConnections, this);
    }

    return isRunServer.load();
}

Server::~Server()
{
    stopServer();

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

    cout << "Port of server: " << port << endl;
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

        auto cmd = [this](int clientSocket, string request) {
            auto sender = [&clientSocket](const char *msg, const size_t size) {
                for(size_t total = 0; total < size; ) {
                    int num = send(clientSocket, (msg + total), (size - total), 0);
                    if(num == -1) {
                        break;
                    }
                    total += num;
                }
            };

            getStdOutFromCommand(request, sender);

            close(clientSocket);
        };

        thread(cmd, clientFd, buffer.data()).detach();
    }
}

void Server::getStdOutFromCommand(const std::string &cmd, sender_t sender)
{
    const int MAX_BUFF = 1024*1024;
    vector<char> buffer(MAX_BUFF);

    const auto deleter = [&](FILE *p) { pclose(p); };
    unique_ptr<FILE, decltype(deleter)> pipe { popen(cmd.c_str(), "r"), deleter};

    if (pipe) {
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != NULL) {
            sender(buffer.data(), strlen(buffer.data()));
        }
    }
}
