#ifndef _SESSION_H
#define _SESSION_H

#include "packet.h"
#include <stdio.h>
#include "bytebuffer.h"

class Session
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
	virtual void onIdle();
	virtual void onError(int errcode);
	virtual void onMessageParseError(Packet *packet);

	Session::State getState() {return state;}
	void setState(Session::State stat) { state = stat; }

	void close();
	void write(const char *data, int dataLen);
protected:
	State state;
	int fd;

	ByteBuffer rBuf;
	ByteBuffer wBuf;

	friend class WorkThread;
};


class SessionFactory
{
public:
	virtual Session *createSession() = 0;
	virtual void releaseSession(Session *sess) = 0;
};

class PacketFactory
{
public:
	virtual Packet *createPacket() = 0;
	virtual void releasePacket(Packet *pack) = 0;
};

#endif
