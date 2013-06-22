#ifndef _TCPACTION_H_
#define _TCPACTION_H_

#include "TcpClient.h"

namespace Network
{

class TcpClient;

/* Basic action class */
class TcpAction
{
public:
	virtual ~TcpAction() {}
public:
	virtual void run(TcpClient*) = 0;
};

/* Close action */
class TcpActionClose: public TcpAction
{
public:
	inline TcpActionClose(UInt32 id_, bool pend_ = false): id(id_), pend(pend_) {}
	virtual ~TcpActionClose() {}
	virtual void run(TcpClient *);
protected:
	UInt32 id;
	bool pend;
};

/* Basic action class with packet support */
class TcpActionWP: public TcpAction
{
public:
	TcpActionWP(std::shared_ptr<std::string>& pkt): packet(pkt) {}
	virtual ~TcpActionWP() {}
protected:
	std::shared_ptr<std::string> packet;
};

/* Send action */
class TcpActionSend: public TcpActionWP
{
public:
	inline TcpActionSend(std::shared_ptr<std::string>& pkt, UInt32 id_, UInt32 gid_): TcpActionWP(pkt), id(id_), gid(gid_) {}
	virtual ~TcpActionSend() {}
	virtual void run(TcpClient *);
protected:
	UInt32 id;
	UInt32 gid;
};

///* Multi-Send action */
//class TcpActionMultiSend: public TcpActionWP
//{
//public:
//	inline TcpActionMultiSend(std::shared_ptr<std::string>& pkt, const std::vector<SessionStruct>& ss_): TcpActionWP(pkt), ss(ss_) {}
//	virtual ~TcpActionMultiSend() {}
//	virtual void run(TcpServer *);
//protected:
//	std::vector<SessionStruct> ss;
//};

/* Broadcast action */
//class TcpActionBroadcast: public TcpActionWP
//{
//public:
//	inline TcpActionBroadcast(std::shared_ptr<std::string>& pkt): TcpActionWP(pkt) {}
//	virtual ~TcpActionBroadcast() {}
//	virtual void run(TcpClient *);
//};

}


#endif // _TCPACTION_H_

