#ifndef _CLIENTNET_H_
#define _CLIENTNET_H_

#include "TcpSocket.h"

//max message size
#define MAX_MESSAGE_SIZE 5*1024      //包的最大长度
#define IN_MAX_MESSAGE_SIZE 10*1024  //接受缓冲区大小
#define IN_MAX_POOL_SIZE 100*1024   //暂存区大小

namespace TcpNetWork
{
	class ClientNet
	{
	public:
		ClientNet();
		~ClientNet();
		bool connect(const char* ServerIP, int ServerPort, int nBlockSec, bool bKeepAlive = false);
		bool sendMsg(void* pBuf, int nSize);
		bool receiveMsg(void* pBuf, int& nSize);
		void runRecvMsg();
		void closeSocket(){return;}
		void drainDataBuf(int len);
		void copyDataBuf(UInt8 *dstSrc, int len);
		//bool OnNetMessage( const UInt16& nOpcode, const UInt8* pDataBuffer, UInt16 nDataSize );
	private:
		// 发送数据缓冲
		UInt8	m_nOutBuff[MAX_MESSAGE_SIZE];	
		int		m_nOutbufLen;   //游标

		UInt8	m_InputBuff[IN_MAX_MESSAGE_SIZE];  //接受缓冲区
		UInt8   m_cbDataBuf[IN_MAX_POOL_SIZE];     //暂存区
		int		m_nInbufLen;                       //接受缓冲区中的有效数据长度     
	private:	
		TcpSocket *m_tcpsocket;
	protected:
		static Utils *utils;
	};
	extern ClientNet _clientnet;
}
#endif