#include "Player.h"

void Player::clear()
{
	ActiveSession::clearSocket();
	m_dbplayer.clear();
	closeSec = 0;
}

bool Player::updateSecond()
{
	if (ActiveSession::updateSec() == false)
	{
		disconnect();
	}

	if (closeSec && --closeSec == 0) {
		return false;
	}


	return true;
}

void Player::disconnect()
{
	closeSec = 60 * 5;
}

void Player::connect()
{
	closeSec = 0;
}