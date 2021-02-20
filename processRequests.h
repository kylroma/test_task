#ifndef PROCESSREQUESTS_H
#define PROCESSREQUESTS_H

#include <vector>
#include <string>

class ProcessRequests
{
public:
    std::string processRequest(const std::string &request);

private:
    std::string getCommand(const std::string &msg);
    std::string getStdoutFromCommand(const std::string &cmd);
};

#endif // PROCESSREQUESTS_H
