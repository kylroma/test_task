#ifndef SERVER_H
#define SERVER_H

#include <atomic>
#include <thread>
#include <functional>

class Server
{
    using sender_t = std::function<void(const char *, const size_t)>;
public:
    Server();
    ~Server();
    bool runServer();

private:
    bool connect();
    void listenConnections();
    void stopServer();

    void getStdOutFromCommand(const std::string &cmd, sender_t sender);

    int mSocet = 0;
    std::atomic_bool isRunServer;
    std::thread mListenThread;
};

#endif // SERVER_H
