#ifndef SERVER_H
#define SERVER_H

#include <atomic>
#include <thread>

class Server
{
public:
    Server();
    ~Server();
    void runServer();
    void stopServer();

private:
    bool connect();
    void listenConnections();

    int mSocet = 0;
    std::atomic_bool isRunServer;
    std::thread mListenThread;
};

#endif // SERVER_H
