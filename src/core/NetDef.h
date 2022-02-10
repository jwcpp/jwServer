#pragma once

#include <memory>

class XSocket;
typedef std::shared_ptr<XSocket> XSocketPtr;


#define MSGID_ACCEPT 10
#define MSGID_CONNECT 11
#define MSGID_CLOSE 12

#define MSGID_PING 20
#define MSGID_PONG 21