#include <string.h>
#include <stdlib.h>
#include "bytebuffer.h"

ByteBuffer::ByteBuffer(int size)
	:curPos(0), endPos(0), buf(NULL), capacity(size)
{
	if (size <= 0)
		capacity = 512;

	resize(capacity);
}

ByteBuffer::ByteBuffer(const ByteBuffer &b)
{
	int n = b.leftSize();
	if (!resize(n))
		return;

	memcpy(buf, b.buf+b.curPos, n);
	curPos = 0;
	endPos = n;
	capacity = n;
}

ByteBuffer::~ByteBuffer()
{
	free(buf);
}

void ByteBuffer::clear()
{
	curPos = endPos = 0;
}

int ByteBuffer::read(void *outBuf, int size)
{
	if (!buf)
		return 0;

	int left = endPos - curPos;
	if (left <= 0)
		return 0;

	if (left > size)
		left = size;

	if (outBuf != NULL) {
		memcpy(outBuf, buf+curPos, left);
	}

	curPos+= left;
	return left;
}

bool ByteBuffer::resize(int newSize)
{
	newSize += 8-newSize%8;
	if (endPos > newSize)
		return false;

	char *newBuf = (char*)realloc(buf, newSize);
	if (!newBuf)
		return false;

	buf = newBuf;
	capacity = newSize;
	return true;
}

int ByteBuffer::append(const void *inBuf, int size)
{
	if (endPos +size > capacity) {
		shrink();
	}

	if (endPos +size > capacity) {
		if (!resize(endPos + size)) {
			return -1;
		}
	}

	memcpy(buf+endPos, inBuf, size);
	endPos += size;
	return size;
}

char *ByteBuffer::peek(int *sz)
{
	if (sz)
		*sz = leftSize();
	return buf+curPos;
}

const char *ByteBuffer::peek(int *sz) const
{
	if (sz)
		*sz = leftSize();
	return buf+curPos;
}

int ByteBuffer::leftSize()
{
	return endPos - curPos;
}

int ByteBuffer::leftSize() const
{
	return endPos - curPos;
}

void ByteBuffer::shrink()
{
	const int threshold = 1024;
	if (curPos > threshold) {
		memmove(buf, buf+curPos, endPos-curPos);
		endPos -= curPos;
		curPos = 0;
		resize(capacity-threshold);
	}
}

