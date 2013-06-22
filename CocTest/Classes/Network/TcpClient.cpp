#include "Config.h"
#include "TcpClient.h"
#include "TcpSocket.h"
#include "TcpAction.h"
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

namespace Network
{
Utils* TcpClientBase::utils = new Utils;
TcpDummyHandler dummyHandler;
bool TcpDummyHandler::onRead( TcpSocket *, struct evbuffer * evbuf )
{
	size_t len = evbuffer_get_length(evbuf);
	if(len > 0)
		evbuffer_drain(evbuf, len);
	return false;
}


struct ClientData
{
	ClientData(TcpClientBase * sv, TcpEventHandler * eh): client(sv), bev(NULL), eventHandler(eh) {}
	~ClientData() { if(bev) bufferevent_free(bev); if(eventHandler) eventHandler->destroy(); }
	TcpClientBase * client;
	struct bufferevent * bev;
	TcpEventHandler * eventHandler;
};
void _onClientRead(struct bufferevent * bev, void * arg)
{
	TcpSocket *sock = static_cast<TcpSocket *>(arg);
	struct evbuffer *evbuf = bufferevent_get_input(bev);
	ClientData *cd = sock->getData<ClientData>();
	while(cd->eventHandler->onRead(sock, evbuf)) { }
}

void _onClientWrite( struct bufferevent * bev, void * arg )
{
	TcpSocket * sock = static_cast<TcpSocket *>(arg);
	if(sock->isPendClose())
	{
		struct evbuffer * evbuf = bufferevent_get_output(bev);
		if(evbuffer_get_length(evbuf) == 0)
			sock->shutdown(2);
	}
}

void TcpClientBase::_onClientEvent( struct bufferevent *, short wht, void * arg )
{
		if((wht & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT)) > 0)
	{
		TcpSocket * sock = static_cast<TcpSocket *>(arg);
		ClientData * data = sock->getData<ClientData>();
		bufferevent_disable(data->bev, EV_WRITE);
		data->eventHandler->onDisconnect(sock);
		TcpClientBase * ts = data->client;
		delete data;
		ts->remove(sock);
	}
}

TcpClientBase::TcpClientBase(struct event_base *eb, bool tsafe): _evBase(eb), _defHandler(&dummyHandler), _running(true), _threadSafe(tsafe), _clientUniqueId(0), _size(0)
{
	_ownBase = (eb == NULL);
	if(eb == NULL)
	{
		_evBase = event_base_new();
		if(_evBase == NULL)
		{
			throw std::bad_alloc();
		}
	}
}

TcpClientBase::~TcpClientBase()
{
	
}

bool TcpClientBase::connect( const std::string& host, UInt16 port, UInt32& sid )
{
	struct bufferevent * bev = bufferevent_socket_new(_evBase, -1, _threadSafe ? BEV_OPT_THREADSAFE : 0);
	if(bev == NULL)
		return false;
	if(bufferevent_socket_connect_hostname(bev, NULL, AF_INET, host.c_str(), port) < 0)
		return false;
	TcpSocket * s = new(std::nothrow) TcpSocket(bufferevent_getfd(bev));
	if(s == NULL)
	{
		bufferevent_free(bev);
		return false;
	}
	ClientData * cd = new(std::nothrow) ClientData(this, _defHandler);
	if(cd == NULL)
	{
		bufferevent_free(bev);
		delete s;
		return false;
	}
	cd->bev = bev;
	bufferevent_setcb(bev, _onClientRead, _onClientWrite, _onClientEvent, s);
	bufferevent_enable(bev, EV_READ | EV_WRITE);
	s->setData(cd);

	sid = static_cast<UInt32>(((++ _clientUniqueId) << 20) + _server.size());
	_server.push_back(s);
	s->setId(sid);
	return true;
}

int TcpClientBase::loop()
{
	_running = true;

	int r = event_base_loop(_evBase, 0);

	_running = false;

	return r;
}


void TcpClientBase::remove( TcpSocket * ts )
{
	//UInt32 index = static_cast<UInt32>(ts->id() & 0xFFFFF);
	//if(index < _clients.size() && _clients[index] == ts)
	//{
	//	_clients[index] = NULL;
	//	if(index == _clients.size() - 1)
	//	{
	//		int idx = static_cast<int>(index) - 1;
	//		while(idx >= 0)
	//		{
	//			if(_clients[idx] != NULL)
	//				break;
	//			_emptySlots.erase(idx);
	//			-- idx;
	//		}
	//		_clients.resize(idx + 1);
	//	}
	//	else
	//		_emptySlots.insert(index);
	//	_size = _clients.size() - _emptySlots.size();
	//}
	delete ts;
}

void TcpClientBase::stop()
{
	_running = false;
}

void TcpClientBase::setHandler( UInt32 id, TcpEventHandler * eh )
{
	UInt32 index = id & 0xFFFFF;
	if(index >= _server.size())
		return;
	TcpSocket * ts = _server[index];
	if(ts != NULL && ts->id() == id)
	{
		TcpEventHandler * old_eh = ts->getData<ClientData>()->eventHandler;
		if(old_eh != eh)
		{
			old_eh->destroy();
			ts->getData<ClientData>()->eventHandler = eh;
		}
	}
}

void TcpClientBase::close2(UInt32 id)
{
	UInt32 index = id & 0xFFFFF;
	if(index >= _server.size())
		return;
	TcpSocket * ts = _server[index];
	if(ts != NULL && ts->id() == id)
		ts->shutdown(2);
}

void TcpClientBase ::pendClose2(UInt32 id)
{
	UInt32 index = id & 0xFFFFF;
	if (index >= _server.size())
		return;
	TcpSocket *ts = _server[index];
	if (ts != NULL && ts->id() == id)
	{
		struct evbuffer *evbuf = bufferevent_get_output(ts->getData<ClientData>()->bev);
		if (evbuffer_get_length(evbuf) == 0)
			ts->shutdown(2);
		else
			ts->pendClose();
	}
}


void TcpClientBase::send2( UInt32 id, const void * buf, int len )
{
	UInt32 index = id & 0xFFFFF;
	if(index >= _server.size())
		return;
	TcpSocket * ts = _server[index];
	if(ts != NULL && ts->id() == id)
	{
		bufferevent_write(ts->getData<ClientData>()->bev, buf, len);
	}
}

void TcpClientBase::send2( UInt32 id, const void * buf1, int len1, const void * buf2, int len2 )
{
	UInt32 index = id & 0xFFFFF;
	if(index >= _server.size())
		return;
	TcpSocket * ts = _server[index];
	if(ts != NULL && ts->id() == id)
	{
		struct bufferevent * bev = ts->getData<ClientData>()->bev;
		bufferevent_write(bev, buf1, len1);
		bufferevent_write(bev, buf2, len2);
	}
}

void TcpClientBase::send2( UInt32 id, const void * buf1, int len1, const void * buf2, int len2, const void * buf3, int len3 )
{
	UInt32 index = id & 0xFFFFF;
	if(index >= _server.size())
		return;
	TcpSocket * ts = _server[index];
	if(ts != NULL && ts->id() == id)
	{
		struct bufferevent * bev = ts->getData<ClientData>()->bev;
		bufferevent_write(bev, buf1, len1);
		bufferevent_write(bev, buf2, len2);
		bufferevent_write(bev, buf3, len3);
	}
}


void TcpClient::close(UInt32 id)
{
	TcpActionClose *ac = new (std::nothrow) TcpActionClose(id); 
	if (ac == NULL)
	{
		pushMsg(ac);
	}
}

int TcpClient::loop()
{
	_evRC = event_new(_evBase, -1, EV_PERSIST, _eventRunCheck, this);
	if (_evRC == NULL)
	{
		return -1;
	}
	struct timeval tv = {0, 50};
	event_add(_evRC, &tv);
	return TcpClientBase::loop();
}

void TcpClient::_eventRunCheck( socket_t, short, void *arg)
{
	static_cast<TcpClient*>(arg)->onRunCheck();
}

void TcpClient::onRunCheck()
{
	if (_running)
	{
		std::vector<TcpAction*> *aq =  fetchMsg();
		if (aq == NULL)
			return;
		for (std::vector<TcpAction*>::iterator it = aq->begin(); it != aq->end(); ++it)
		{
			if (*it != NULL)
			{
				(*it)->run(this);
			}
			
		}
	}
	else
	{
		event_base_loopbreak(_evBase);
	}
}

}