#include "Player.h"

void Player::Init(char symbol, PlayerType playerType)
{
	if (playerType == PlayerType::smartMove)
	{
		_smart = SmartPlayer();
	}
	_symbol = symbol;
}

void Player::Reset()
{
	if (_smart)
	{
		_smart->Reset();
	}
}

void Player::ResetSmart()
{
	_smart.reset();
}