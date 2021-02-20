#include "processRequests.h"
#include <stdio.h>
#include <memory>

#include <iostream> // ????
#include <thread>
#include <chrono>
using namespace std;

string ProcessRequests::processRequest(const std::string &request)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    string cmd = getCommand(request);
    return getStdoutFromCommand(cmd);
}

std::string ProcessRequests::getCommand(const std::string &msg)
{
    size_t found = msg.find("/ HTTP/");
    if(found != std::string::npos) {
        return string(msg, 0, found);
    }

    return msg;
}

string ProcessRequests::getStdoutFromCommand(const std::string &cmd)
{
    string out;
    const int MAX_BUFF = 1024*4;
    vector<char> buffer(MAX_BUFF);

    const auto deleter = [&](FILE *p) { pclose(p); };
    unique_ptr<FILE, decltype(deleter)> pipe { popen(cmd.c_str(), "r"), deleter};

    if (pipe) {
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != NULL) {
            out += buffer.data();
        }
    }
    return out;
}
