#include "tcpserver.h"
#include "packet.h"
#include <string.h>
#include <iostream>


class EchoPacket : public Packet
{
public:
	int tryParse(const ByteBuffer *buf)
	{
		int n;
		const char *p = buf->peek(&n);
		const void *x = memchr(p, '\n', n);
		if (x) {
			return (char*)x - p +1;
		}

		return 0;
	}

	bool parse(const char *buf, int len)
	{
		data.append(buf, len);
		return true;
	}

	bool toBuffer(ByteBuffer *out)
	{
		int n;
		const char *p = data.peek(&n);
		out->append(p, n);

		data.read(NULL, n);
		data.shrink();
		return true;
	}

	std::string getValue()
	{
		int n;
		char *x = data.peek(&n);
		return std::string(x, n);
	}

protected:
	ByteBuffer data;
};


class EchoSession : public Session
{
public:
	void onMessageReceived(Packet *pack)
	{
		EchoPacket *p = (EchoPacket*)pack;
		std::string s = p->getValue();
		write(s.c_str(), s.length());
	}

	void onClosed()
	{
		printf("closed\n");
	}
};


class EchoSessionFactory : public SessionFactory
{
public:
	Session *createSession()
	{
		return new EchoSession;
	}

	void releaseSession(Session *sess)
	{
		delete sess;
	}
};

class EchoPacketFactory : public PacketFactory
{
public:
	Packet *createPacket()
	{
		return new EchoPacket;
	}

	void releasePacket(Packet *pack)
	{
		delete pack;
	}
};


int main()
{
	EchoSessionFactory sessFactory;
	EchoPacketFactory packFactory;
	TcpServer s(4, &sessFactory, &packFactory);
	s.addListener(8088);
	s.serve();
}
