#include <iostream>
#include <Windows.h>
#include <thread>
#include <chrono>

#include "Menu.h"
#include "Game.h"
#include "Console.h"

void Menu::Init(std::shared_ptr<Game> game, std::shared_ptr<Console> console)
{
	_game = game;
	_console = console;

	SetGameBoardSize();
	SetGameMode();
}

void Menu::SetGameBoardSize()
{
	const std::string message = "Please, choose the size of the board:";

	_choiceMessages.emplace_back("3x3");
	_choiceMessages.emplace_back("4x4");
	_choiceMessages.emplace_back("5x5");

	Submenu(message);

	switch (_pointer._line)
	{
	case 0: _game.lock()->SetBoardSize(3); break;
	case 1: _game.lock()->SetBoardSize(4); break;
	case 2: _game.lock()->SetBoardSize(5); break;
	}

	_choiceMessages.clear();
}

void Menu::SetGameMode()
{
	const std::string message = "Please, choose the game mode:";

	_choiceMessages.emplace_back("Computer vs. Computer");
	_choiceMessages.emplace_back("Player vs. Computer");
	_choiceMessages.emplace_back("Player vs. Player");

	Submenu(message);

	switch (_pointer._line)
	{
	case 0: _game.lock()->SetGameMode(GameMode::computerVScomputer); break;
	case 1: _game.lock()->SetGameMode(GameMode::playerVScomputer); break;
	case 2: _game.lock()->SetGameMode(GameMode::playerVSplayer); break;
	}

	_choiceMessages.clear();
}

void Menu::Submenu(const std::string& message)
{
	_pointer._line = 0;

	while (!(GetAsyncKeyState(VK_RETURN) & 0x01))
	{
		_console->SetPosition(0, 0);

		std::cout << message << std::endl;

		for (int i = 0; i < _choiceMessages.size(); ++i)
		{
			if (_pointer._line == i)
			{
				std::cout << _pointer._left << _choiceMessages[i] << _pointer._right << std::endl;
			}
			else
			{
				std::cout << "   " << _choiceMessages[i] << "   " << std::endl;
			}
		}

		if ((GetAsyncKeyState(VK_UP) & 0x01) && (_pointer._line != 0))
		{
			--_pointer._line;
		}
		if ((GetAsyncKeyState(VK_DOWN) & 0x01) && (_pointer._line < _choiceMessages.size() - 1))
		{
			++_pointer._line;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	Refresh();
}

void Menu::Refresh()
{
	system("cls");
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
}
