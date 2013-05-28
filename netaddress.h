#ifndef NET_ADDRESS_H
#define NET_ADDRESS_H

#include <string>

class NetAddress
{
public:
	NetAddress(const char *host);
	NetAddress(const char *host, u16 port);
	NetAddress(const struct sockaddr_in &addr);
	NetAddress(const struct sockaddr_in6 &addr);
	NetAddress(const struct sockaddr_storage &addr);

	const struct sockaddr_storage &toSockAddr() const;
	struct sockaddr_storage &toSockAddr();

	const struct in6_addr &a6() const;
	const struct in_addr &a4() const;
	u16 getPort() const;
	void setPort(u16 port);

	bool isV6() const {return af()==AF_INET6;} 

	std::string toString() const;

	int af() const;

	NetAddress &operator=(const char *host);
	NetAddress &operator=(const struct sockaddr_in &addr);
	NetAddress &operator=(const struct sockaddr_in6 &addr);
	NetAddress &operator=(const struct sockaddr_storage &addr);

protected:
	bool aton(const char *host, u16 port);

protected:
	bool hasPort;
	struct sockaddr_storage ss;
};

#endif
