#pragma once

#include <memory>

enum GameMode
{
	computerVScomputer,
	playerVScomputer,
	playerVSplayer
};

class Console;
class Menu;
class Board;

class Game : public std::enable_shared_from_this<Game>
{
public:
	Game();
	~Game();

	void Start();

	void SetBoardSize(int boardSize) { _boardSize = boardSize; }
	void SetGameMode(GameMode gameMode) { _mode = gameMode; }

private:
	void Init();
	void GameLoop();
	void PrintFinalMessage();

	void Input();
	void Update();
	void Render();

	std::shared_ptr<Console>	_console;
	std::shared_ptr<Menu>		_menu;
	std::shared_ptr<Board>		_board;
	GameMode					_mode;
	int							_boardSize = 0;
};
