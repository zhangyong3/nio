#ifndef _BYTE_BUF_H
#define _BYTE_BUF_H

#include <stdio.h>

class ByteBuffer
{
public:
	explicit ByteBuffer(int size=128);
	ByteBuffer(const ByteBuffer &buf);

	virtual ~ByteBuffer();

	void clear();

	int read(void *buf, int size);
	int append(const void *buf, int size);

	char *peek(int *sz = NULL);
	const char *peek(int *sz) const;

	int leftSize();
	int leftSize() const;

	void shrink();

protected:
	bool resize(int newSize);

protected:
	int curPos;
	int endPos;
	char *buf;
	int capacity;
};
#endif
