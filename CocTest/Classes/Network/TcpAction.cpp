#include "Config.h"
#include "TcpAction.h"

namespace Network
{

void TcpActionClose::run( TcpClient *ts )
{
	if(pend)
		ts->pendClose2(id);
	else
		ts->close2(id);
}

void TcpActionSend::run( TcpClient *ts )
{
	if(packet->empty())
		return;
	if(gid == 0xFFFFFFFF)
		ts->send2(id, &(*packet)[0], static_cast<int>(packet->size()));
	else
	{
		UInt16 cnt = 1;
		ts->send2(id, &cnt, 2, &gid, sizeof(UInt32), &(*packet)[0], static_cast<int>(packet->size()));
	}
}

//void TcpActionBroadcast::run( TcpClient *ts )
//{
//	if(packet->empty())
//		return;
//	ts->broadcast2(&(*packet)[0], static_cast<int>(packet->size()));
//}

//void TcpActionMultiSend::run( TcpClient *ts )
//{
//	std::map<UInt32, std::vector<UInt32> > targets;
//	for(auto it = ss.begin(); it != ss.end(); ++ it)
//	{
//		if(it->gid == 0xFFFFFFFF)
//			ts->send2(it->sid, &(*packet)[0], static_cast<int>(packet->size()));
//		else
//			targets[it->sid].push_back(it->gid);
//	}
//	for(auto it = targets.begin(); it != targets.end(); ++ it)
//	{
//		UInt16 cnt = static_cast<UInt16>(it->second.size());
//		ts->send2(it->first, &cnt, 2, &it->second[0], static_cast<int>(sizeof(UInt32) * cnt), &(*packet)[0], static_cast<int>(packet->size()));
//	}
//}

}

