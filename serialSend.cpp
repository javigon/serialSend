#include <stdio.h>
#include <stdlib.h>

#include <iostream>
using namespace std;

#include "serialSendLib.h"

int main(int argc, char ** argv)
{
    QString dev = argv[1];
    int baudrate = atoi(argv[2]);
    int dataBits = atoi(argv[3]);
    QString parity = argv[4];
    QString stop = argv[5];
    bool softwareHandshake = atoi(argv[6]);
    bool hardwareHandshake = atoi(argv[7]);
    bool openRead = atoi(argv[8]);
    bool openWrite = atoi(argv[9]);
    bool applyNow = atoi(argv[10]);
    QString filename = argv[11];
    unsigned int charDelay = atoi(argv[12]);
    std::string logFile = (std::string)argv[13];
    
    //int baudrate = 115200;
    //int dataBits = 8;
    //QString parity = "none";
    //QString stop = "1";
    //bool softwareHandshake = false;
    //bool hardwareHandshake = false;
    //bool openRead = true;
    //bool openWrite = true;
    //bool applyNow = true;
    
    // Create a serialSend object
    serialSend *app = new serialSend();
    // Establish a serial connection. See parameters in serialSendLib.cpp
    app->manualConnectTTY(dev, baudrate, dataBits, parity, stop, softwareHandshake, hardwareHandshake, openRead, openWrite, applyNow);
    //Sleep 1 seconds
    sleep(1);
    // Send a file we selected and delay between sent bytes (microseconds)
    app->manualSendFile(filename, charDelay);
    // Close serial connection 
    app->manualDisconnectTTY();
    exit(1);
}
