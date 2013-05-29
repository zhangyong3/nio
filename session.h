#ifndef _SESSION_H
#define _SESSION_H

#include "packet.h"
#include <stdio.h>
#include <stdint.h>
#include "bytebuffer.h"
#include "thread.h"
#include "linkedlist.h"

class WorkThread;
class Session : public Node
{
public:
	typedef enum {CLOSED, READ, WRITE} State;

public:
	Session();
	virtual ~Session();

	void reset();

	virtual void onOpened();
	virtual void onMessageReceived(Packet *packet);
	virtual void onClosed();

	//return true to persist, false to close
	virtual bool onIdle();
	virtual void onError(int errcode);
	virtual void onMessageParseError(Packet *packet);

	Session::State getState() {return state;}
	void setState(Session::State stat) { state = stat; }

	void close();
	void write(const char *data, int dataLen);

protected:
	uint64_t lastActiveTime;
	State state;
	int fd;

	ByteBuffer rBuf;
	ByteBuffer wBuf;
	WorkThread *thread;

	friend class WorkThread;
};

class SessionFactory
{
public:
	virtual Session *createSession() = 0;
	virtual void releaseSession(Session *sess) = 0;
	virtual int getSessionTimeout() { return -1; }
};

class PacketFactory
{
public:
	virtual Packet *createPacket() = 0;
	virtual void releasePacket(Packet *pack) = 0;
};


class WorkThread : public Thread
{
public:
	WorkThread(SessionFactory *sessionFactory, PacketFactory *packetFactory);
	~WorkThread();
	void run();

protected:
	void close(Session *sess);

protected:
	SessionFactory *sessFactory;
	PacketFactory *packFactory;
	int pfd[2];
	int epfd;

	LinkedList sessions;

	friend class TcpServer;
	friend class Session;
};

#endif
