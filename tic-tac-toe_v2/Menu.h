#pragma once

#include <string>

class Game;
class Console;

class Menu
{
public:
	void Init(std::shared_ptr<Game> game, std::shared_ptr<Console> console);

private:
	void GetRoundAmountToPlay();
	void ProcessInput();
	bool IsInteger(const std::string& number);

	std::weak_ptr<Game>			_game;
	std::shared_ptr<Console>	_console;
};
