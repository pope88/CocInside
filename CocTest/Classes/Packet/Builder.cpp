#include "Config.h"
#include "Builder.h"
#include "../TcpNetwork/ClientNet.h"

namespace Packet
{
void Builder::send()
{
	if (stream.get() != NULL || pack())
	{
		TcpNetWork::_clientnet.sendMsg(&(*stream)[0], static_cast<int>(stream->size()));
	}
}

const std::string& Builder::data()
{
	static const std::string empty;
	if (stream.get() != NULL || pack())
	{
		return *stream.get();
	}
	return empty;
}

bool Builder::pack()
{
	std::string *s = new(std::nothrow) std::string(6, 0);
	if (s == NULL)
	{
		return false;
	}
	stream.reset(s);
	*((UInt16*)&((*s)[4])) =  op;
	packContent();
	size_t sz = s->size() - 6;
	memcpy(&(*s)[0], &sz, 3);
	return true;
}

bool Builder::repack()
{
	return pack();
}
}