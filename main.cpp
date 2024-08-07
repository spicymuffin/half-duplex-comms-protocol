#include <iostream>

#include "TelemetrySystem.h"

using namespace std;

int main(int argc, char* argv[])
{
    while (true)
    {
        CommunicationSystem ts("CommunicationSystem", 1000, nullptr);
        ts.update();
    }
}