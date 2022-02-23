#pragma once

#include <vector>
#include <string>

class Game;
class Console;

class Menu
{
public:
	void Init(std::shared_ptr<Game> game, std::shared_ptr<Console> console);

private:
	void SetGameBoardSize();
	void SetGameMode();

	void Submenu(const std::string& message);

	void Refresh();

	struct Pointer
	{
		const std::string _left = ">> ";
		const std::string _right = " <<";
		int _line = 0;
	};

	std::weak_ptr<Game>			_game;
	std::shared_ptr<Console>	_console;
	std::vector<std::string>	_choiceMessages;
	Pointer						_pointer;
};
