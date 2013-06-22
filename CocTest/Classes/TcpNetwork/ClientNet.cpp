#include "Config.h"
#include "ClientNet.h"

namespace TcpNetWork
{
ClientNet _clientnet;
Utils *ClientNet::utils = new Utils;
ClientNet::ClientNet():m_nOutbufLen(0), m_nInbufLen(0), m_tcpsocket(NULL)
{
	memset(m_nOutBuff, 0, sizeof(m_nOutBuff));
	memset(m_InputBuff, 0, sizeof(m_InputBuff));
	memset(m_cbDataBuf, 0, sizeof(m_cbDataBuf));
}

ClientNet::~ClientNet()
{
	memset(m_nOutBuff, 0, sizeof(m_nOutBuff));
	memset(m_InputBuff, 0, sizeof(m_InputBuff));
	if (m_tcpsocket != NULL)
	{
		delete m_tcpsocket;
	}
}

bool ClientNet::connect(const char* ServerIP, int ServerPort, int nBlockSec, bool bKeepAlive)
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

bool  ClientNet::sendMsg(void* pBuf, int nSize)
{
	if(pBuf ==  NULL|| nSize <= 0) 
	{ 
		return false; 
	} 

	if (m_tcpsocket == NULL || m_tcpsocket->getFD() == INVALID_SOCKET)
	{ 
		return false; 
	} 

	// ���ͨѶ��Ϣ������ 
	int packsize = 0; 
	packsize = nSize; 

	if ( packsize <= 0 || packsize > MAX_MESSAGE_SIZE )
	{
		return false;
	}
	else
	{
		m_tcpsocket->write(pBuf, packsize);
	}
	return true; 
}



void ClientNet::runRecvMsg()
{
	if (m_tcpsocket == NULL || m_tcpsocket->getFD() == INVALID_SOCKET)
	{
		return;
	}
	
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

			// ���������ݻ���
			CopyMemory(m_cbDataBuf, m_InputBuff, nPacketSize);

			// �ӽ��ջ����Ƴ�
			MoveMemory(m_InputBuff, m_InputBuff+nPacketSize, sizeof(m_InputBuff) - nPacketSize);
			m_nInbufLen -= nPacketSize;

			//�ַ���
			// 

			// �������ݰ�����Ӧ�ò�����߼�����
		    //pHead = (PktHdr*) (m_cbDataBuf);
			//const UInt16 nDataSize = nPacketSize - (UInt16)sizeof(PktHdr);
			//OnNetMessage(pHead->op, m_cbDataBuf+sizeof(PktHdr), nDataSize);

			++nCount;
		}
	}
}

void ClientNet::drainDataBuf(int len)
{
	if (len > 0)
	{
		memset(m_cbDataBuf, 0, len);
		memmove(m_cbDataBuf, m_cbDataBuf + len, sizeof(m_cbDataBuf) - len);
	}
}

void ClientNet::copyDataBuf(UInt8 *dstSrc, int len)
{
	memmove(dstSrc, m_cbDataBuf, len);
}


}


