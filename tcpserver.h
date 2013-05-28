#ifndef _TCP_SERVER_H
#define _TCP_SERVER_H

#include <vector>
#include <string>
#include "thread.h"
#include "session.h"

class TcpServer
{
public:
	TcpServer(int threads, SessionFactory *sessFactory, PacketFactory *packetFactory,  int maxConn=1024);
	virtual ~TcpServer();

	bool addListener(int port);
	bool addListener(const std::string &host, int port);

	virtual bool initialize() {}

	void serve();

	void setDaemon(bool enable) { daemon = enable; }

protected:
	void openSocket();

protected:
	std::vector<Thread *> threads;
	std::vector<std::pair<std::string, int> > addrs;
	std::vector<int> socks;

	static void sigHandler(int sig);
	bool daemon;

	int epfd;

public:
	static bool running;
};

#endif
