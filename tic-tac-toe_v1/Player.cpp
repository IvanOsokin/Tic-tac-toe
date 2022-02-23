#include "Player.h"

void Player::Init(char charToSet)
{
	_char = charToSet;
}

void Player::Init(char charToSet, ArtifitialIntelligenceData AIData)
{
	_AIData = AIData;
	_char = charToSet;
}

void Player::Reset()
{
	if (_AIData)
	{
		_AIData->Reset();
	}
}