#include <iostream>
#include "server.h"
#include <string>

using namespace std;

int main()
{
    Server s;
    s.runServer();

    string cmd;
    while (cmd != "exit") {
        cin >> cmd;
    }

    s.stopServer();
    cout << "Hello World!" << endl;
    return 0;
}
