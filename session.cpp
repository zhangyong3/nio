#include <unistd.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include "session.h"
#include "tcpserver.h"

static uint64_t currentTimeMillis()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return 1000*(uint64_t)tv.tv_sec+ tv.tv_usec/1000;
}


Session::Session()
	:fd(-1), thread(NULL), lastActiveTime(0)
{
	state = CLOSED;
}


Session::~Session()
{
	close();
}

void Session::reset()
{
	close();
	thread = NULL;
	rBuf.clear();
	wBuf.clear();
	lastActiveTime = 0;
}

void Session::onOpened()
{
}

void Session::onMessageReceived(Packet *packet)
{
}

void Session::onMessageParseError(Packet *packet)
{
}

void Session::onClosed()
{
}

bool Session::onIdle()
{
	return true;
}

void Session::onError(int errcode)
{
}

void Session::close()
{
	if (fd != -1) {
		if (thread != NULL) {
			thread->close(this);
		}

		::close(fd);
		fd = -1;
		state = CLOSED;
	}
}

void Session::write(const char *data, int dataLen)
{
	wBuf.append(data, dataLen);
}


//WorkThread
WorkThread::WorkThread(SessionFactory *sessionFactory, PacketFactory *packetFactory)
{
	sessFactory = sessionFactory;
	packFactory = packetFactory;
	epfd = epoll_create(1024);
	pipe(pfd);
}

WorkThread::~WorkThread()
{
	::close(epfd);
	::close(pfd[0]);
	::close(pfd[1]);
}


void WorkThread::close(Session *sess)
{
	struct epoll_event ev;
	ev.events = EPOLLIN|EPOLLOUT;
	ev.data.ptr = sess;
	epoll_ctl(epfd, EPOLL_CTL_DEL, sess->fd, &ev);
}

void WorkThread::run()
{
	struct epoll_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.events = EPOLLIN;
	ev.data.ptr = (void*)-1;
	epoll_ctl(epfd, EPOLL_CTL_ADD, pfd[0], &ev);

	const int MAX_EVENTS = 128;
	struct epoll_event events[MAX_EVENTS];
	while (TcpServer::running) {
		int n =  epoll_wait(epfd, events, MAX_EVENTS, 1);
		if (n == -1) {
			perror("epoll_pwait");
			continue;
		}

		for (int i = 0; i < n; ++i) {
			if (events[i].data.ptr == (void*)-1) {
				int fd;
				int ret = read(pfd[0], &fd, sizeof(fd));
				if (ret != 4) {
					perror("read");
					continue;
				}

				int flags = 0;
				if ( (flags = fcntl(fd, F_GETFL, 0)) < 0 ||
					fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
					perror("setting O_NONBLOCK");
					::close(fd);
					continue;
				}

				Session *new_session = sessFactory->createSession();
				if (!new_session) {
					::close(fd);
				} else {
					new_session->fd = fd;
					new_session->state = Session::READ;
					new_session->thread = this;

					new_session->onOpened();
					memset(&ev, 0, sizeof(ev));
					ev.events = EPOLLIN;
					ev.data.ptr = new_session;
					epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

					if (sessFactory->getSessionTimeout() != -1) {
						new_session->lastActiveTime = currentTimeMillis();
						sessions.insertRear(new_session);
					}
				}
			} else {
				char buf[1024];
				int sz = 0;
				Session *sess = (Session*)events[i].data.ptr;
				if (sess->getState() == Session::READ) {
					sz = read(sess->fd, buf, sizeof(buf));
					if (sz <= 0) {
						if (sz == 0) {
							sess->onClosed();
						} else if (sz == -1 && errno != EINTR) {
							sess->onError(errno);
						}

						sess->close();
						sessFactory->releaseSession(sess);
						continue;
					} else {
						sess->rBuf.append(buf, sz);
						Packet *pack = packFactory->createPacket();
						if (pack) {
							int ret = pack->parse(&sess->rBuf);

							if (ret == Packet::PARSE_OK) {
								sess->onMessageReceived(pack);
							} else if (ret == Packet::PARSE_FAILURE) {
								sess->onMessageParseError(pack);
							}

							packFactory->releasePacket(pack);
						}

						if (sessFactory->getSessionTimeout() != -1) {
							sessions.deleteNode(sess);
							sess->lastActiveTime = currentTimeMillis();
							sessions.insertRear(sess);
						}
					}

					if (sess->wBuf.leftSize() > 0) {
						ev.events = EPOLLOUT;
						ev.data.ptr = sess;
						epoll_ctl(epfd, EPOLL_CTL_MOD, sess->fd, &ev);
						sess->state = Session::WRITE;
					}
				} else if (sess->getState() == Session::WRITE) {
					while ( (sz = sess->wBuf.read(buf, sizeof(buf))) > 0) {
						write(sess->fd, buf, sz);
					}
					sess->wBuf.shrink();

					ev.events = EPOLLIN;
					ev.data.ptr = sess;
					epoll_ctl(epfd, EPOLL_CTL_MOD, sess->fd, &ev);
					sess->state = Session::READ;
				}
			}
		}

		if (sessFactory->getSessionTimeout() != -1) {
			uint64_t deadLine = currentTimeMillis() - sessFactory->getSessionTimeout();
			Session *sess = (Session*)sessions.getHead();
			while (sess != NULL) {
				if (sess->lastActiveTime > deadLine)
					break;

				if (!sess->onIdle()) {
					Session *p = (Session*)sess->getNext();
					sessions.deleteNode(sess);
					sess->close();
					sessFactory->releaseSession(sess);
					sess = p;
				} else {
					sess->lastActiveTime = currentTimeMillis();
					sess = (Session*)sess->getNext();
				}
			}
		}
	}
}
