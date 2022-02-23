#include <iostream>

#include "Console.h"
#include "Game.h"
#include "Menu.h"

void Menu::Init(std::shared_ptr<Game> game, std::shared_ptr<Console> console)
{
	_game = game;
	_console = console;

	GetRoundAmountToPlay();
}

void Menu::GetRoundAmountToPlay()
{
	ProcessInput();
}

void Menu::ProcessInput()
{
	std::string userInput;

	do
	{
		_console->CleanConsole();
		_console->SetPosition(0, 0);
		std::cout << "Please, enter the amount of rounds to play: ";

		std::getline(std::cin, userInput);
	}while (!IsInteger(userInput));

	_game.lock()->SetRoundAmount(std::stoi(userInput));
}

bool Menu::IsInteger(const std::string& number)
{
	if (number == "")
	{
		return false;
	}

	for (int i = 0; i < number.length(); ++i)
	{
		if (!std::isdigit(number[i]))
		{
			return false;
		}
	}
	return true;
}