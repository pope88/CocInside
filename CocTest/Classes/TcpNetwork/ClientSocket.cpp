#include "Config.h"
#include "ClientSocket.h"

namespace TcpNetWork
{
Utils *ClientSocket::utils = new Utils;
ClientSocket::ClientSocket():m_nInbufLen(0), m_nInbufStart(0), m_tcpsocket(NULL)
{
	memset(m_InputBuff, 0, sizeof(m_InputBuff));
}

ClientSocket::~ClientSocket()
{
	memset(m_InputBuff, 0, sizeof(m_InputBuff));
	if (m_tcpsocket != NULL)
	{
		delete m_tcpsocket;
	}
}

void ClientSocket::drainBuf(UInt8 *beginBuf, UInt16 Len)
{
	if (Len  > (sizeof(m_InputBuff)- m_nInbufLen))
	{
		memmove(beginBuf, );
	}
	else
	{
		memmove(beginBuf);
	}
}

bool ClientSocket::connect(const char* ServerIP, int ServerPort, int nBlockSec, bool bKeepAlive)
{
	// 检查参数
	if(ServerIP == NULL || strlen(ServerIP) > 15)
	{
		return false;
	}

	if (m_tcpsocket != NULL)
	{
		SAFE_DELETE(m_tcpsocket);
	}

	m_tcpsocket = new TcpSocket();
	if (m_tcpsocket == NULL || !m_tcpsocket->initialized())
	{
		SAFE_DELETE(m_tcpsocket);
	}

	m_tcpsocket->makeBlock(true);

	if (m_tcpsocket->connect(ServerIP, ServerPort))
	{
		m_tcpsocket->makeLinger(true);

		m_tcpsocket->makeBlock(false);

		m_tcpsocket->makeNoDelay(true);

		timeval timeout;
		timeout.tv_sec	= nBlockSec;
		timeout.tv_usec	= 0;
		fd_set writeset, exceptset;
		FD_ZERO(&writeset);
		FD_ZERO(&exceptset);
		FD_SET(m_tcpsocket->getFD(), &writeset);
		FD_SET(m_tcpsocket->getFD(), &exceptset);

		int ret = select(FD_SETSIZE, NULL, &writeset, &exceptset, &timeout);
		if (ret == 0 || ret < 0) 
		{
			closeSocket();
			return false;
		} 
		else // ret > 0
		{
			ret = FD_ISSET(m_tcpsocket->getFD(), &exceptset);
			if(ret)		// or (!FD_ISSET(m_sockClient, &writeset)
			{
				closeSocket();
				return false;
			}
		}
	}
	else
	{
		SAFE_DELETE(m_tcpsocket);
		return false;
	}
	return true;
}

bool  ClientSocket::sendMsg(void* pBuf, int nSize)
{
	//if(pBuf ==  NULL|| nSize <= 0) 
	//{ 
	//	return false; 
	//} 

	//if (m_tcpsocket == NULL || m_tcpsocket->getFD() == INVALID_SOCKET)
	//{ 
	//	return false; 
	//} 

	//// 检查通讯消息包长度 
	//int packsize = 0; 
	//packsize = nSize; 

	//// 检测BUF溢出 
	//if(m_nOutbufLen + nSize > MAX_MESSAGE_SIZE) 
	//{ 
	//	// 立即发送OUTBUF中的数据，以清空OUTBUF。 
	//	Flush(); 
	//	if(m_nOutbufLen + nSize > MAX_MESSAGE_SIZE) 
	//	{ 
	//		// 出错了 
	//		Destroy(); 
	//		return false; 
	//	} 
	//} 
	//// 数据添加到BUF尾 
	//memcpy(m_nOutBuff + m_nOutbufLen, pBuf, nSize); 
	//m_nOutbufLen += packsize; 
	return true; 
}

bool ClientSocket::receiveMsg(void* pBuf, int& nSize)
{
	if (pBuf == NULL || nSize <= 0)
	{
		return false;
	}

	if (m_tcpsocket == NULL || m_tcpsocket->getFD() == INVALID_SOCKET)
	{
		return false;
	}

	if (m_nInbufLen < 2)
	{
		if (!recvFromSock() || m_nInbufLen < 2)
		{
			return false;
		}
	}

	int packsize = (UInt8)m_InputBuff[m_nInbufStart] + (UInt8)m_InputBuff[(m_nInbufStart+1)%IN_MAX_MESSAGE_SIZE] * 256;
	if (packsize <= 0 || packsize > IN_MAX_MESSAGE_SIZE)
	{
		m_nInbufLen = 0;
		m_nInbufStart = 0;
		return false;
	}

	if (packsize > m_nInbufLen)
	{
		if (!recvFromSock() || packsize > m_nInbufLen)
		{
			return false;
		}
	}

	if (m_nInbufStart + packsize > IN_MAX_MESSAGE_SIZE)
	{
		int copylen = IN_MAX_MESSAGE_SIZE - m_nInbufStart;
		memcpy(pBuf, m_InputBuff + m_nInbufStart, copylen);
		memcpy((UInt8*)pBuf+copylen, m_InputBuff, packsize - copylen);
		nSize = packsize;
	}
	else
	{
		memcpy(pBuf, m_InputBuff + m_nInbufStart, packsize);
		nSize = packsize;
	}

	// 重新计算环形缓冲区头部位置 
	m_nInbufStart = (m_nInbufStart + packsize) % IN_MAX_MESSAGE_SIZE; 
	m_nInbufLen -= packsize;        //bugs
	return  true;
}

// 从网络中读取尽可能多的数据，实际向服务器请求数据的地方 
bool ClientSocket::recvFromSock(void) 
{ 

	if (m_nInbufLen >= IN_MAX_MESSAGE_SIZE || m_tcpsocket == NULL || m_tcpsocket->getFD() == INVALID_SOCKET)
	{ 
		return false; 
	} 

	// 接收第一段数据 
	int savelen, savepos;           // 数据要保存的长度和位置 
	if(m_nInbufStart + m_nInbufLen < IN_MAX_MESSAGE_SIZE) 
	{   // INBUF中的剩余空间有回绕 
		savelen = IN_MAX_MESSAGE_SIZE - (m_nInbufStart + m_nInbufLen);        // 后部空间长度，最大接收数据的长度 
	}
	else 
	{ 
		savelen = IN_MAX_MESSAGE_SIZE - m_nInbufLen; 
	} 

	// 缓冲区数据的末尾 
	savepos = (m_nInbufStart + m_nInbufLen) % IN_MAX_MESSAGE_SIZE; 
	//CHECKF(savepos + savelen <= IN_MAX_MESSAGE_SIZE); 
	int inlen = recv(m_tcpsocket->getFD(), (char*)(m_InputBuff + savepos), savelen, 0); 
	if(inlen > 0) 
	{ 
		// 有接收到数据 
		m_nInbufLen += inlen; 

		if (m_nInbufLen > IN_MAX_MESSAGE_SIZE) 
		{ 
			return false; 
		} 

		// 接收第二段数据(一次接收没有完成，接收第二段数据) 
		if(inlen == savelen && m_nInbufLen < IN_MAX_MESSAGE_SIZE)
		{ 
			int savelen = IN_MAX_MESSAGE_SIZE - m_nInbufLen; 
			int savepos = (m_nInbufStart + m_nInbufLen) % IN_MAX_MESSAGE_SIZE; 
			//CHECKF(savepos + savelen <= IN_MAX_MESSAGE_SIZE); 
			inlen = recv(m_tcpsocket->getFD(),(char*)(m_InputBuff + savepos), savelen, 0); 
			if(inlen > 0)
			{ 
				m_nInbufLen += inlen; 
				if (m_nInbufLen > IN_MAX_MESSAGE_SIZE) 
				{ 
					return false; 
				}    
			}
			else if(inlen == 0)
			{ 
				Destroy(); 
				return false; 
			}
			else
			{ 
				// 连接已断开或者错误（包括阻塞） 
				if (hasError())
				{ 
					Destroy(); 
					return false; 
				} 
			} 
		} 
	} 
	else if(inlen == 0) 
	{ 
		Destroy(); 
		return false; 
	}
	else
	{ 
		// 连接已断开或者错误（包括阻塞） 
		if (hasError()) 
		{ 
			Destroy(); 
			return false; 
		} 
	} 
	return true; 
} 

void ClientSocket::runRecvMsg()
{
	int nCount = 0;
	for (;;)
	{
		// 接收数据
		int nRecvSize = m_tcpsocket->read((char*)(m_InputBuff+m_nInbufLen), sizeof(m_InputBuff)-m_nInbufLen);
			//::recv(m_tcpsocket->getFD(), (char*)(m_InputBuff+m_nInbufLen), sizeof(m_InputBuff)-m_nInbufLen, 0);
		if (nRecvSize <= 0 && ( m_tcpsocket->lastErr() != WEINPROGRESS &&  m_tcpsocket->lastErr() != WEWOULDBLOCK))
		{
			std::cout << "服务器主动断开连接!" << std::endl;
			break;
		}
		else if (nRecvSize <= 0 && ( m_tcpsocket->lastErr() == WEINPROGRESS || m_tcpsocket->lastErr() == WEWOULDBLOCK) )
		{
			std::cout << "等待接受数据!" << std::endl;
			break;
		}

      #pragma pack(push, 1)
		struct PktHdr
		{
			UInt32 size;
			UInt16 op;
		} ATTR_PACKED();
      #pragma pack(pop)

		// 保存已经接收数据的大小
		m_nInbufLen += nRecvSize;

		// 接收到的数据够不够一个包头的长度
		while (m_nInbufLen >= 6)
		{
			// 读取包头
			PktHdr* pHead = (PktHdr*) (m_InputBuff);
			const UInt16 nPacketSize = pHead->size & 0x00FFFFFF;

			// 判断是否已接收到足够一个完整包的数据
			if (m_nInbufLen < nPacketSize)
			{
				// 还不够拼凑出一个完整包
				break;
			}

			//// 拷贝到数据缓存
			//memmove(m_cbDataBuf, m_cbRecvBuf, nPacketSize);

			// 从接收缓存移除
			memmove(m_InputBuff, m_InputBuff+nPacketSize, sizeof(m_InputBuff)-nPacketSize);
			MoveMemory(m_cbRecvBuf, m_cbRecvBuf+nPacketSize, m_nRecvSize);
			m_nRecvSize -= nPacketSize;


			// 分派数据包，让应用层进行逻辑处理
		//	pHead = (PktHdr*) (m_cbDataBuf);
			//const UInt16 nDataSize = nPacketSize - (UInt16)sizeof(PktHdr);
			//OnNetMessage(pHead->op, m_cbDataBuf+sizeof(PktHdr), nDataSize);

			++nCount;
		}
	}
}

bool ClientSocket::OnNetMessage( const UInt16& nOpcode, const UInt8* pDataBuffer, UInt16 nDataSize )
{
	//switch (nOpcode)
	//{
	//case NET_TEST1:
	//	{
	//		NetPacket_Test1* pMsg = (NetPacket_Test1*) pDataBuffer;
	//			return OnNetPacket(pMsg);
	//	}
	//	break;

	//default:
	//	{
	//		std::cout << "收取到未知网络数据包:" << nOpcode << std::endl;
	//		return false;
	//	}
	//	break;
	//}
	return true;
}

}