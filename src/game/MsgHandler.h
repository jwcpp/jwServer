#pragma once

class Session;
class NetPacket;
class MsgHandler
{
public:
	void dispense(Session* session, NetPacket& packet);

	template<int ID>
	void process(Session* session, NetPacket& packet);
	template<class MSG>
	void process(Session* session, MSG& msg);
};