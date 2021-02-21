#include <iostream>
#include "server.h"
#include <string>

using namespace std;

int main()
{
    Server s;
    if(s.runServer()) {
        cout << "To close the application write \"exit\" or \"q\"\n";
        string cmd;
        
        while (true) {
            cin >> cmd;
            if(cmd == "exit" || cmd == "q") {
                break;
            }
        }
    }

    return 0;
}
