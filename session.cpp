#include "session.h"
#include <unistd.h>


Session::Session()
	:fd(-1)
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
	rBuf.clear();
	wBuf.clear();
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

void Session::onIdle()
{
}

void Session::onError(int errcode)
{
}

void Session::close()
{
	if (fd != -1) {
		::close(fd);
		fd = -1;
		state = CLOSED;
	}
}

void Session::write(const char *data, int dataLen)
{
	wBuf.append(data, dataLen);
}
