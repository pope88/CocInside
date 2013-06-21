#include "Config.h"
#include "Tcp.h"
//#include "Game.h"
//#include "Network/TcpSocket.h"
#include "Network/TcpClient.h"
//#include "Packet/Processor.h"
//#include "Packet/Builder.h"
//#include "Object/Player.h"
//#include "KtCfg.h"
#include <event2/bufferevent.h>

namespace Worker
{

Tcp tcp;

 class InitHandler: public Network::TcpEventHandler
 {
 public:
 	virtual bool onAccept(Network::TcpSocket * sock)
 	{
 		return true;
 	}
 	virtual bool onRead(Network::TcpSocket * ts, struct evbuffer * evbuf)
 	{
 		//return game.processor()->parseInit(evbuf, ts->id(), ts->getRemoteAddr());
		return true;
 	}
 	virtual void onDisconnect(Network::TcpSocket *)
 	{
 	}
 };

// class PlayerHandler: public Network::TcpEventHandler
// {
// public:
// 	PlayerHandler(Object::Player * p): player(p) {}
// 	virtual ~PlayerHandler() {}
// 	virtual void destroy() { delete this; }
// 	virtual bool onRead(Network::TcpSocket * ts, struct evbuffer * evbuf)
// 	{
// 		return game.processor()->parsePlayer(evbuf, player);
// 	}
// 	virtual void onDisconnect(Network::TcpSocket * ts)
// 	{
// 		game.processor()->pushDisconnect(ts->id(), player);
// 	}
// 	virtual bool acceptBroadcast(struct bufferevent *) { return player->online(); }
// private:
// 	Object::Player * player;
// };

// class GatewayHandler: public Network::TcpEventHandler
// {
// public:
// 	virtual bool onRead(Network::TcpSocket * ts, struct evbuffer * evbuf)
// 	{
// 		return game.processor()->parseGateway(evbuf, ts->id());
// 	}
// 	virtual void onDisconnect(Network::TcpSocket * ts)
// 	{
// 		game.processor()->pushGatewayDisconnect(ts->id());
// 	}
// 	virtual bool acceptBroadcast(struct bufferevent * bev)
// 	{
// 		const UInt16 targetCnt = 0;
// 		bufferevent_write(bev, &targetCnt, 2);
// 		return true;
// 	}
// };

bool Tcp::init()
{
	/*_server = new(std::nothrow) Network::TcpServer(_evBase);
	if(_server == NULL)
		return false;

	for(auto it = ktCfg.serverName.begin(); it != ktCfg.serverName.end(); ++ it)
	{
		Network::TcpSocket sock;
		sock.makeBlock(true);
		if(sock.connect("t2.ktpd.xindong.com", 7777))
		{
			std::string n;
			n.resize(it->length() + 2);
			memcpy(&n[0], &ktCfg.tcpPort, 2);
			memcpy(&n[2], &(*it)[0], it->length());
			sock.write(&n[0], static_cast<int>(n.length()));
			sock.close();
		}
	}
	_running = true;
	static InitHandler initHandler;
	_server->setDefaultHandler(&initHandler);
	_server->listen(ktCfg.tcpPort);
	addTimer(-1, 50);*/

	_client = new(std::nothrow) Network::TcpClient(_evBase);
	if (_client == NULL)
	{
		return false;
	}
	_running =true;
	static InitHandler initHandler;
	_client->setDefaultHandler(&initHandler);
	UInt32 socketID = 0;
	_client->connect("127.0.0.1", 8880, socketID);
	addTimer(-1,50);
	return true;
}

void Tcp::uninit()
{
	//_running = false;
	//delete _server;
}

void Tcp::stop()
{
	//if(_running)
	//	_server->stop();
	//Base::stop();
}

void Tcp::close( UInt32 id, UInt32 gid )
{
	//if(_running)
	//{
	//	if(gid == 0xFFFFFFFF)
	//		_server->close(id);
	//	else
	//	{
	//		Packet::GatewayCommand gc;
	//		gc.setCommand(1);
	//		gc.send(id, gid);
	//	}
	//}
}

void Tcp::pendClose( UInt32 id )
{
	//if(_running)
	//	_server->pendClose(id);
}

void Tcp::send( UInt32 id, UInt32 gid, std::shared_ptr<std::string>& pkt )
{
	//if(_running)
	//	_server->send(id, gid, pkt);
}

/*void Tcp::sendMulti( void * multi, std::shared_ptr<std::string>& pkt )
{
	//if(_running)
	//	_server->send(*reinterpret_cast<std::vector<Network::SessionStruct> *>(multi), pkt);
}*/

void Tcp::sendNolock( UInt32 id, UInt32 gid, std::shared_ptr<std::string>& pkt )
{
	//if(_running)
	//	_server->sendNoLock(id, gid, pkt);
}

void Tcp::sendLock( )
{
	//_server->sendLock();
}

void Tcp::sendUnlock( )
{
	//_server->sendUnlock();
}

/*void Tcp::broadcast( std::shared_ptr<std::string>& pkt )
{
// 	if(_running)
// 		_server->broadcast(pkt);
}*/

//void Tcp::setPlayer( UInt32 id, Object::Player * player )
//{
// 	if(_running)
// 	{
// 		PlayerHandler * ph = new(std::nothrow) PlayerHandler(player);
// 		_server->setHandler(id, ph);
// 	}
// 	else
// 	{
// 		_server->close(id);
// 	}
//}

void Tcp::setFromGateway( UInt32 id )
{
// 	if(_running)
// 	{
// 		static GatewayHandler gatewayHandler;
// 		_server->setHandler(id, &gatewayHandler);
// 	}
// 	else
// 	{
// 		_server->close(id);
// 	}
}

void Tcp::onTimer( int id, void * )
{
 	switch(id)
 	{
 	case -1:
 		_client->onRunCheck();
 		break;
 	}
}

}
