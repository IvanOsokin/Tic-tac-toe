#include <iostream>

#include "Game.h"
#include "Console.h"
#include "Menu.h"
#include "Board.h"

Game::Game()
	: _console(std::make_shared<Console>())
	, _menu(std::make_shared<Menu>())
	, _board(std::make_shared<Board>())
	, _mode(GameMode::computerVScomputer)
{}

Game::~Game() = default;

void Game::Start()
{
	Init();
	GameLoop();
	PrintFinalMessage();
}

void Game::Init()
{
	_console->Init();
	_menu->Init(shared_from_this(), _console);

	auto matrixSizeForBoard = _boardSize * 2 - 1;
	_board->Init(_console, matrixSizeForBoard, _mode);
}

void Game::GameLoop()
{
	while (!_board->ShouldClose())
	{
		Input();
		Update();
		Render();
	}
	_board->DrawBoard2();
}


void Game::PrintFinalMessage()
{
	COORD printPosition{ 0, static_cast<SHORT>(_boardSize * 2) };

	while (!(GetAsyncKeyState(VK_RETURN) & 0x01))
	{
		_console->SetPosition(printPosition.X, printPosition.Y);
		std::cout << "Game Over!" << std::endl;
	};
}

void Game::Input()
{
	_board->Input();
}

void Game::Update()
{
	_board->Update();
}

void Game::Render()
{
	_board->Render();
}
