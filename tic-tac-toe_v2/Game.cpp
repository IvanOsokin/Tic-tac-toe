#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>

#include "Game.h"
#include "Console.h"
#include "Menu.h"
#include "Board.h"

Game::Game()
	: _console(std::make_shared<Console>())
	, _menu(std::make_shared<Menu>())
	, _board(std::make_shared<Board>())
{}

Game::~Game() = default;

void Game::Start()
{
	PreInit();

	while (_currentRound < _roundAmount)
	{
		++_currentRound;

		GameLoop();

		_board->Reset();
	}
	UploadResult();
	PrintFinalMessage();
}

void Game::PreInit()
{
	_menu->Init(shared_from_this(), _console);

	_console->Init();
	_console->SetConsoleParam();

	_board->PreInit(_console, _boardSize);
}

void Game::GameLoop()
{
	Init();

	PrintInfo();

	while (!_board->ShouldClose())
	{
		Update();
		Render();
	}

	if (_board->GetWinner() != WinLine::noWinner)
	{
		++_smartWon;
		return;
	}

	++_draw;
}

void Game::Init()
{
	_console->CleanConsole();

	_board->Init();
}

void Game::Update()
{
	_board->Update();
}

void Game::Render()
{
	_board->Render();
}

void Game::PrintFinalMessage()
{
	PrintInfo();

	COORD printPosition{ 0, static_cast<SHORT>(_boardSize * 2 + 5) };

	while (GetKeyState(VK_RETURN) >= 0)
	{
		_console->SetPosition(printPosition.X, printPosition.Y);
		std::cout << "All the games have been played. Press Enter." << std::endl;
	};
}

void Game::PrintInfo()
{
	_console->SetPosition(0, 6);
	std::cout << "Smart player won - " << _smartWon;

	_console->SetPosition(22, 6);
	std::cout << "Draw - " << _draw;

	_console->SetPosition(0, 8);
	std::cout << "Number of rounds - " << _roundAmount;

	_console->SetPosition(0, 9);
	std::cout << "Current round - " << _currentRound;
}

void Game::UploadResult()
{
	std::time_t time = std::time(nullptr);

	std::ofstream fout;
	
	fout.open("result.csv", std::ios::app);

	if (fout.fail())
	{
		std::cout << "Something went wrong." << std::endl;
		std::cout << "Couldn't write result down" << std::endl;
		return;
	}

	fout << "\"" << std::asctime(std::localtime(&time)) << "\";" << std::endl;
	fout << "\"Number of rounds played\";\"" << _roundAmount << "\";" << std::endl;
	fout << "\"Smart player won\";\"" << _smartWon << "\";" << std::endl;
	fout << "\"Draw\";\"" << _draw << "\";" << std::endl << std::endl;

	fout.close();
}