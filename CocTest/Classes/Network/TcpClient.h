#ifndef _TCPCLIENT_H_
#define _TCPCLIENT_H_

#include "Network/Utils.h"
#include "../../System/MsgQueue.h"

struct event_base;
struct event;
struct evbuffer;
struct bufferevent;
namespace Network
{

class TcpSocket;
class TcpAction;
/* Tcp socket event handler */
class TcpEventHandler
{
public:
	virtual ~TcpEventHandler() {}
	/* Destroy self, which is used to avoid destruction on static objects by leaving it empty */
	virtual void destroy() {}
	/* Called when data inputs (used by client socket) */
	virtual bool onRead(TcpSocket *, struct evbuffer *) { return false; }
	/* Called when client socket disconnects (used by client socket) */
	virtual void onDisconnect(TcpSocket *) {}
	/* Indicates that if this socket accepts global broadcasts */
	virtual bool acceptBroadcast(struct bufferevent *) { return false; }
};

class TcpDummyHandler: public TcpEventHandler
{
	virtual bool onRead(TcpSocket *, struct evbuffer *);
};
extern TcpDummyHandler dummyHandler; 

/* Basic TCP-Client Class */
class TcpClientBase
{
public:
	/* Constructor
	 *   if evBase is NULL, the class will create a event base and release it on destruction
	 *   if evBase is not NULL, the class will use it to handle client and incoming sockets
	 *   threadSafe indicates wheather client sockets require thread safe functions in libevent2 */
	TcpClientBase(struct event_base * evBase, bool threadSafe);
	/* Destructor */
	virtual ~TcpClientBase();
	/* Connect to host:port */
	bool connect(const std::string& host, UInt16 port, UInt32& sid);
	/* Start event loop, only used when event base is created by itself, Otherwise please run
	 * event loop in the caller procedure */
	virtual int loop();

	/* Stop running the event loop */
	void stop();

	/* Set the default handler which is used when a new connection is accepted */
	inline void setDefaultHandler(TcpEventHandler * eh) { _defHandler = eh; }
	/* Get the default handler */
	inline TcpEventHandler * getDefaultHandler() { return _defHandler; }
	/* Set a new handler for a connection */
	void setHandler(UInt32 id, TcpEventHandler * eh);
	/* Get current handler for the connection */
	TcpEventHandler * getHandler(UInt32 id);

	inline UInt32 size() { return _size; }

protected:
	//bool doListen(TcpListener *);
	void onError();
	void remove(TcpSocket *);
protected:
	//static void _onListenerEvent( socket_t, short, void * );
	static void _onClientEvent( struct bufferevent *, short, void * );

protected:
	//std::vector<TcpListener *> _listener;
	struct event_base * _evBase;
	std::vector<struct event *> _connectEvent;
	std::vector<TcpSocket *> _server;
	//std::set<UInt32> _emptySlots;
	TcpEventHandler * _defHandler;
	bool _running;
	bool _ownBase;
	bool _threadSafe;
	UInt32 _clientUniqueId;
	UInt32 _size;

protected:
	/* Used by friend classes only */
	void close2(UInt32);
	void pendClose2(UInt32);
	void send2(UInt32, const void *, int);
	void send2(UInt32, const void *, int, const void *, int);
	void send2(UInt32, const void *, int, const void *, int, const void *, int);
	//void broadcast2(const void *, int);
	//void broadcast2(const void *, int, const void *, int);

protected:
	static Utils * utils;
};

/* Basic TCP-Client Class */
class TcpClient : public TcpClientBase, public System::MsgQueue<TcpAction *>
{
	friend class TcpActionClose;
	friend class TcpActionSend;
public:
	inline TcpClient(struct event_base * eb = NULL): TcpClientBase(eb, true), _evRC(NULL) {}
	void close(UInt32 id);
	void pendClose(UInt32 id, UInt32 gid, std::shared_ptr<std::string>& pkt);
	//void send( std::vector<SessionStruct>& ss, std::shared_ptr<std::string>& pkt );
	void sendLock();
	void sendUnLock();
	void sendNoLock(UInt32 id, UInt32 gid, std::shared_ptr<std::string>& pkt );
	//void broadcast( std::shared_ptr<std::string>& pkt );
    
	/* Start event loop, only used when event base is created by itself, Otherwise please run
	 * event loop in the caller procedure
	 * this virtual implementation adds timer to handle queued packets/actions */
	virtual int loop();

	void onRunCheck();
private:
	static void _eventRunCheck( socket_t, short, void * );

protected:
	struct event * _evRC;
};

}
#endif