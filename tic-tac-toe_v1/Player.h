#pragma once

#include <optional>

using lineNumber = int;

struct ArtifitialIntelligenceData
{
	enum WhereMaxAmount
	{
		somewhere,
		column,
		row,
		mainDiagonal,
		auxDiagonal,
	};

	ArtifitialIntelligenceData()
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

class Player
{
public:
	void Init(char charToSet);
	void Init(char charToSet, ArtifitialIntelligenceData AIData);

	void Reset();

	char GetChar() const { return _char; }

	std::optional<ArtifitialIntelligenceData> _AIData;
	
private:
	char _char = '\0';
};