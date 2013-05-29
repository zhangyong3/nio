#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "tcpserver.h"
#include "session.h"

volatile bool TcpServer::running = true;

void TcpServer::sigHandler(int sig)
{
	if (sig == SIGTERM) {
		TcpServer::running = false;
	}
}

TcpServer::TcpServer(int numThread, SessionFactory *sessFactory, PacketFactory *packetFactory, int maxConn)
	: daemon(false), epfd(-1)
{
	srand(time(NULL));
	for (int i = 0;i < numThread; ++i) {
		Thread *thr = new WorkThread(sessFactory, packetFactory);
		threads.push_back(thr);
	}
	signal(SIGTERM, sigHandler);

	struct rlimit rlim;
	int status = getrlimit(RLIMIT_NOFILE, &rlim);
	if (status == 0) {
		rlim.rlim_cur = rlim.rlim_max = maxConn;
		if (setrlimit(RLIMIT_NOFILE, &rlim) != 0) {
			printf("set nofile error\n");
		}
	}
}

TcpServer::~TcpServer()
{
	for (std::vector<Thread*>::iterator it = threads.begin();
			it != threads.end(); ++it) {
		(*it)->join();
	}

	if (epfd != -1)
		close(epfd);
}

bool TcpServer::addListener(int port)
{
	return addListener("", port);
}

bool TcpServer::addListener(const std::string &host, int port)
{
	addrs.push_back(std::make_pair(host, port));
	return true;
}

void TcpServer::openSocket()
{
	for (std::vector<std::pair<std::string, int> >::iterator it = addrs.begin();
			it != addrs.end() ; ++it) {
		int fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd < 0) continue;

		struct sockaddr_in sa;
		memset(&sa, 0, sizeof(sa));

		sa.sin_family = AF_INET;
		sa.sin_port = htons(it->second);
		if (it->first.length() == 0) {
			sa.sin_addr.s_addr = INADDR_ANY;
		} else {
			sa.sin_addr.s_addr = inet_addr(it->first.c_str());
		}

		int on = 1;
		setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

		if (bind(fd, (struct sockaddr*)&sa, sizeof(sa)) != 0) {
			perror("bind");
			close(fd);
			continue;
		}

		if (listen(fd, 16) != 0) {
			perror("listen");
			close(fd);
			continue;
		}

		socks.push_back(fd);
	}

	epfd = epoll_create(10);//socks.size());
	for (std::vector<int>::iterator it = socks.begin(); it != socks.end(); ++it) {
		struct epoll_event ev;
		memset(&ev, 0, sizeof(ev));
		ev.events = EPOLLIN;
		ev.data.fd = *it;
		epoll_ctl(epfd, EPOLL_CTL_ADD, *it, &ev);
	}
}

void TcpServer::serve()
{
	initialize();
	openSocket();
	if (threads.size() == 0 || socks.size() == 0) {
		printf("no listen socket opened\n");
		return;
	}

	for (std::vector<Thread*>::iterator it = threads.begin();
			it != threads.end(); ++it) {
		(*it)->start();
	}

	int cur = 0;
	struct epoll_event events[8];
	while (TcpServer::running) {
		int n  = epoll_wait(epfd, events, 8, 100);
		if (n <= 0) continue;

		for (int i = 0; i < n; ++i) {
			int fd = accept(events[i].data.fd, NULL, NULL);
			if (fd < 0) continue;

			cur = (cur+1) % threads.size();
			WorkThread *thr = (WorkThread*)threads[cur];
			write(thr->pfd[1], &fd, sizeof(fd));
		}
	}
}
