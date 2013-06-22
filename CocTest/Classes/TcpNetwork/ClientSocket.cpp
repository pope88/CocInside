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
	// ������
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

	//// ���ͨѶ��Ϣ������ 
	//int packsize = 0; 
	//packsize = nSize; 

	//// ���BUF��� 
	//if(m_nOutbufLen + nSize > MAX_MESSAGE_SIZE) 
	//{ 
	//	// ��������OUTBUF�е����ݣ������OUTBUF�� 
	//	Flush(); 
	//	if(m_nOutbufLen + nSize > MAX_MESSAGE_SIZE) 
	//	{ 
	//		// ������ 
	//		Destroy(); 
	//		return false; 
	//	} 
	//} 
	//// ������ӵ�BUFβ 
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

	// ���¼��㻷�λ�����ͷ��λ�� 
	m_nInbufStart = (m_nInbufStart + packsize) % IN_MAX_MESSAGE_SIZE; 
	m_nInbufLen -= packsize;        //bugs
	return  true;
}

// �������ж�ȡ�����ܶ�����ݣ�ʵ����������������ݵĵط� 
bool ClientSocket::recvFromSock(void) 
{ 

	if (m_nInbufLen >= IN_MAX_MESSAGE_SIZE || m_tcpsocket == NULL || m_tcpsocket->getFD() == INVALID_SOCKET)
	{ 
		return false; 
	} 

	// ���յ�һ������ 
	int savelen, savepos;           // ����Ҫ����ĳ��Ⱥ�λ�� 
	if(m_nInbufStart + m_nInbufLen < IN_MAX_MESSAGE_SIZE) 
	{   // INBUF�е�ʣ��ռ��л��� 
		savelen = IN_MAX_MESSAGE_SIZE - (m_nInbufStart + m_nInbufLen);        // �󲿿ռ䳤�ȣ����������ݵĳ��� 
	}
	else 
	{ 
		savelen = IN_MAX_MESSAGE_SIZE - m_nInbufLen; 
	} 

	// ���������ݵ�ĩβ 
	savepos = (m_nInbufStart + m_nInbufLen) % IN_MAX_MESSAGE_SIZE; 
	//CHECKF(savepos + savelen <= IN_MAX_MESSAGE_SIZE); 
	int inlen = recv(m_tcpsocket->getFD(), (char*)(m_InputBuff + savepos), savelen, 0); 
	if(inlen > 0) 
	{ 
		// �н��յ����� 
		m_nInbufLen += inlen; 

		if (m_nInbufLen > IN_MAX_MESSAGE_SIZE) 
		{ 
			return false; 
		} 

		// ���յڶ�������(һ�ν���û����ɣ����յڶ�������) 
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
				// �����ѶϿ����ߴ��󣨰��������� 
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
		// �����ѶϿ����ߴ��󣨰��������� 
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
		// ��������
		int nRecvSize = m_tcpsocket->read((char*)(m_InputBuff+m_nInbufLen), sizeof(m_InputBuff)-m_nInbufLen);
			//::recv(m_tcpsocket->getFD(), (char*)(m_InputBuff+m_nInbufLen), sizeof(m_InputBuff)-m_nInbufLen, 0);
		if (nRecvSize <= 0 && ( m_tcpsocket->lastErr() != WEINPROGRESS &&  m_tcpsocket->lastErr() != WEWOULDBLOCK))
		{
			std::cout << "�����������Ͽ�����!" << std::endl;
			break;
		}
		else if (nRecvSize <= 0 && ( m_tcpsocket->lastErr() == WEINPROGRESS || m_tcpsocket->lastErr() == WEWOULDBLOCK) )
		{
			std::cout << "�ȴ���������!" << std::endl;
			break;
		}

      #pragma pack(push, 1)
		struct PktHdr
		{
			UInt32 size;
			UInt16 op;
		} ATTR_PACKED();
      #pragma pack(pop)

		// �����Ѿ��������ݵĴ�С
		m_nInbufLen += nRecvSize;

		// ���յ������ݹ�����һ����ͷ�ĳ���
		while (m_nInbufLen >= 6)
		{
			// ��ȡ��ͷ
			PktHdr* pHead = (PktHdr*) (m_InputBuff);
			const UInt16 nPacketSize = pHead->size & 0x00FFFFFF;

			// �ж��Ƿ��ѽ��յ��㹻һ��������������
			if (m_nInbufLen < nPacketSize)
			{
				// ������ƴ�ճ�һ��������
				break;
			}

			//// ���������ݻ���
			//memmove(m_cbDataBuf, m_cbRecvBuf, nPacketSize);

			// �ӽ��ջ����Ƴ�
			memmove(m_InputBuff, m_InputBuff+nPacketSize, sizeof(m_InputBuff)-nPacketSize);
			MoveMemory(m_cbRecvBuf, m_cbRecvBuf+nPacketSize, m_nRecvSize);
			m_nRecvSize -= nPacketSize;


			// �������ݰ�����Ӧ�ò�����߼�����
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
	//		std::cout << "��ȡ��δ֪�������ݰ�:" << nOpcode << std::endl;
	//		return false;
	//	}
	//	break;
	//}
	return true;
}

}