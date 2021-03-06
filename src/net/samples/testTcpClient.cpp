/* This file is part of mediaserver. A webrtc sfu server.
 * Copyright (C) 2018 Arvind Umrao <akumrao@yahoo.com> & Herman Umrao<hermanumrao@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include "base/base.h"
#include "base/logger.h"
#include "base/application.h"
#include "net/netInterface.h"
#include "net/TcpConnection.h"
#include "base/test.h"
#include "base/time.h"
#include "base/platform.h"

using std::endl;
using namespace base;
using namespace net;
using namespace base::test;


class tesTcpClient {
public:

    tesTcpClient() {}

    void start(std::string ip, int port) {

        // socket.send("Arvind", "127.0.0.1", 7331);
        tcpClient = new TcpConnectionBase();

        tcpClient->Connect(ip, port);
        const char snd[6] = "12345";
        std::cout << "TCP Client send data: " << snd << "len: " << strlen((const char*) snd) << std::endl << std::flush;

        tcpClient->send(snd, 5);
        

    }

    void shutdown() {
        // socket.send("Arvind", "127.0.0.1", 7331);
        delete tcpClient;
        tcpClient = nullptr;

    }

    void on_close(Listener* connection) {

    
        std::cout << " Close Con LocalIP" << connection->GetLocalIp() << " PeerIP" << connection->GetPeerIp() << std::endl << std::flush;

    }

    void on_read(Listener* connection, const char* data, size_t len) {
        std::cout << "data: " << data << "len: " << len << std::endl << std::flush;
        std::string send = "12345";
        connection->send((const char*) send.c_str(), 5);

    }
   // TcpConnection *tcpClient; // do not use this this is for RTP
    TcpConnectionBase *tcpClient;

};

int main(int argc, char** argv) {
    Logger::instance().add(new ConsoleChannel("debug", Level::Trace));


        Application app;

        tesTcpClient socket;


	int port = 51038;
	std::string ip = "192.168.0.19";
	std::string filename;
	    
	if (argc > 1) {
	   filename = argv[1];
	}

	if (argc > 2) {
	   ip = argv[2];
	 }

	 if (argc > 3) {
	    port = atoi(argv[3]);
	 }

        socket.start(ip, port);


        // app.waitForShutdown([&](void*) {
        //     socket.shutdown();

        // });


        base::sleep(250);
        
        socket.shutdown();

    return 0;
}
