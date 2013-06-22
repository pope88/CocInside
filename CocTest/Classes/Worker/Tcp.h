#ifndef _WORKER_TCP_H_
#define _WORKER_TCP_H_

#include "Base.h"

// namespace Object
// {
// 	class Player;
// }

 namespace Network
 {
 	class TcpClient;
 	class TcpEventHandler;
 }

namespace Worker
{

class Tcp: public Base
{
public:
	Tcp(){}//:_client(NULL) {}
	void stop();
	void close(UInt32, UInt32);
	void pendClose(UInt32);
	void send(UInt32, UInt32, std::shared_ptr<std::string>&);
	//void sendMulti(void *, std::shared_ptr<std::string>&);
	void sendNolock(UInt32, UInt32, std::shared_ptr<std::string>&);
	void sendLock();
	void sendUnlock();
//	void broadcast(std::shared_ptr<std::string>&);
//	void setPlayer(UInt32, Object::Player *);
	void setFromGateway(UInt32);

protected:
	virtual bool init();
	virtual void uninit();
	virtual const char * getLogFilename() { return "tcp"; }
	virtual void onTimer(int, void *);

private:
	bool _running;
	Network::TcpClient *_client;
};

extern Tcp tcp;

}

#endif // _WORKER_TCP_H_
