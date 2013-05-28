#include <iostream>
#include "thread.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class WorkThread: public Thread
{
public:
	WorkThread()
		: nSocket(500)
	{

	}

	void run()
	{
		std::cout << "begin test" <<std::endl;

		int fds[nSocket];
		int i;

		for (i = 0; i < nSocket; ++i) {
			fds[i] = socket(AF_INET, SOCK_STREAM, 0);
			if (fds[i] < 0) {
				perror("open");
				continue;
			}

			struct sockaddr_in addr;
			memset(&addr, 0, sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_port = htons(8088);
			addr.sin_addr.s_addr = inet_addr("127.0.0.1");

			if (connect(fds[i], (struct sockaddr*)&addr, sizeof(addr)) != 0) {
				perror("connect");
				close(fds[i]) ;
				fds[i] = -1;
				continue;
			}
		}

		std::cout << "ok" <<std::endl;

		char buf[32];
		while (true) {
			int broken = 0;
			for (i = 0; i < nSocket; ++i) {
				if (fds[i] < 0)  {
					++broken;
					continue;
				}

				int n = sprintf(buf , "%u\n", rand());
				write(fds[i], buf, n);
				if (read(fds[i], buf, n) <= 0) {
					close(fds[i]);
					fds[i] = 0;
				}
			}

			if (broken == nSocket)
				break;
		}
	}

	int nSocket;
};


int main()
{
	int n = 2;
	int i;
	WorkThread *p = new WorkThread[n];
	//WorkThread p[2];

	for (i= 0; i < n; ++i) {
		if (!p[i].start())
			printf("not start\n");
	}

	for (i= 0; i < n; ++i)
		p[i].join();
}
