#ifndef _PACKET_H
#define _PACKET_H

#include "bytebuffer.h"

class Packet
{
public:
	enum {
		PARSE_OK = 0,
		PARSE_FAILURE = -1,
		PARSE_NEED_MORE_DATA = -2,
	};

public:
	Packet() {}
	virtual ~Packet() {}

	//return value
	//<=0, need more data
	//> 0, ok
	virtual int tryParse(const ByteBuffer *buf) = 0;


	//return true if parse ok, otherwise false
	virtual bool parse(const char *buf, int len) = 0;

	virtual bool toBuffer(ByteBuffer *out) = 0;
public:

	int parse(ByteBuffer *buf)
	{
		int len = tryParse(buf);
		if (len <= 0)
			return PARSE_NEED_MORE_DATA;
		bool ret = parse(buf->peek(NULL), len);
		buf->read(NULL, len);
		buf->shrink();
		return ret? PARSE_OK: PARSE_FAILURE;
	}
};
#endif
