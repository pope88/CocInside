#ifndef _HANDLER_H_
#define _HANDLER_H_
namespace Packet
{
	class Handler;
	struct HandlerMsgHeader
	{
		Handler *handler;
		UInt32 size;
	};

	struct HandlerMsgInit
	{
		HandlerMsgHeader hdr;
		UInt32 sessionId;
		UInt32 fromGateway;
		UInt32 remoteAddr;
	};

	struct HandlerMsgPlayer
	{
		HandlerMsgHeader hdr;
		union
		{
			//Object::Player * player;
			struct
			{
				UInt32 playerId;
				UInt16 op;
			} gateway;
		};
	};

	class Handler
	{
	public:
		inline Handler(UInt16 op, UInt8 type);
		virtual void handle(HandlerMsgHeader *) = 0;
	};

	template<class T, class DT, UInt16 HOP, UInt8 TF = 1>
	class HandlerT: public Handler
	{
	public: HandlerT<T, DT, HOP, TF>() : Handler(HOP, T) {}
	protected:
		virtual void handle(HandlerMsgHeader * hdr)
		{
			DT pktObj;
			if (pktObj.unpack(UInt8*)hdr + static_cast<T*>(this)->getDataBufferSize(), hdr->size)
			{
				static_cast<T *>(this)->process(pktObj, hdr);
			}
		}
	};

#define  HANDLER_CLASS(n, o) \
	namespace Packet { \
	class Handler##n: public HandlerT<Handler##n, SC##n##o, o> \
	{ \
	public: \
	inline size_t getDataBuffSize() { return sizeof(HandlerMsgPlayer); } \
	inline void process(SC##n##_##o &pkt, HandlerMsgHeader *hdr) \
	{ \
	    Object::Player *player = ((HandlerMsgPlayer *)hdr)->player; \
	    if(player == NULL) return; \


#define  HANDLER_CLASS_INIT(n, o) \
	namespace Packet { \
	class Handler##n: public Handler<Handler##n, SC##n##_##o, o, 2> \
	{ \
	public:\
	inline size_t getDataBuffSize() { return sizeof(HandlerMsgInit); } \
	inline void process(SC##n##_##o &pkt, HandlerMsgHeader *hdr) \
    { \
        HandlerMsgInit * msgInit__ = (HandlerMsgInit *)hdr; \
        UInt32 sessionId = msgInit__->sessionId; \
        UInt32 gatewayId = msgInit__->fromGateway; \
	    UInt32 remoteAddr = msgInit__->remoteAddr; \

#define MSG_QUERY_ARGS(at, av) 

#define HANDLER_END(n) \
	 } \
   }; \
static Handler##n __static_initializer_##n; \

}
#endif