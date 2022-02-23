#pragma once

#include <optional>

using lineNumber = int;

struct SmartPlayer
{
	enum WhereMaxAmount
	{
		somewhere,
		column,
		row,
		mainDiagonal,
		auxDiagonal,
	};

	SmartPlayer()
		: _own(MaxCharacterAmount())
		, _enemy(MaxCharacterAmount())
	{}

	void Reset()
	{
		_own.Reset();
		_enemy.Reset();
	}

	struct MaxCharacterAmount
	{
		MaxCharacterAmount()
			: _where(std::make_pair(WhereMaxAmount::somewhere, 0))
		{}

		void Reset()
		{
			_where.first = somewhere;
			_where.second = 0;
			_howMuch = 0;
		}

		std::pair<WhereMaxAmount, lineNumber>	_where;
		int										_howMuch = 0;
	};

	MaxCharacterAmount	_own;
	MaxCharacterAmount	_enemy;
};

enum PlayerType
{
	smartMove,
	randomMove,
};

class Player
{
public:
	void Init(char symbol, PlayerType playerType);

	void Reset();
	void ResetSmart();
	char GetChar() const { return _symbol; }

	std::optional<SmartPlayer> _smart;

private:
	char _symbol = '\0';
};