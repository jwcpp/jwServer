#pragma once

// output to struct
template<typename MSG>
inline bool parsePro(MSG& msg, const char* buff, int size)
{
	return msg.ParseFromArray(buff, size);
}

template<typename MSG>
inline bool parsePro(MSG& msg, NetPacket & packet)
{
	return msg.ParseFromArray(packet.contents(), packet.wpos());
}

// output to buffer
template<typename MSG>
inline NetPacket serialize(int type, MSG& msg)
{
	int size = msg.ByteSizeLong();
	NetPacket packet(type, size, ByteBuffer::Resize{});
	msg.SerializeToArray((void*)packet.contents(), size);

	return packet;
}


//Test test;
//socket->sendPacket(serialize(eMSGID_SessionVerify, test));


/*
class Test
{
public:
	void ParseFromArray(uint8_t* data, int size) {
		memcpy(&num, data, sizeof(int));
	}

	int num = 0;
};
*/

#define SELECT_MSG_ID(msgid) \
case msgid:\
	process<msgid>(session, packet);\
break;

#define SELECT_MSG_OBJ(msgid, MSG) \
case msgid:\
{\
	MSG msg;\
	msg.ParseFromArray(packet.contents(), packet.wpos());\
	process<MSG>(session, msg);\
}break;

#define SWITCH_MSG_BEGIN(msgid) switch(msgid){
#define SWITCH_MSG_END() default: break;}

