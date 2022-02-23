#pragma once

#include <memory>

class Console;
class Menu;
class Board;

class Game : public std::enable_shared_from_this<Game>
{
public:
	Game();
	~Game();

	void Start();

	void SetRoundAmount(int roundAmount) { _roundAmount = roundAmount; }

private:
	void PreInit();
	void GameLoop();
	void Init();
	
	void Update();
	void Render();

	void PrintFinalMessage();
	void PrintInfo();

	void UploadResult();

	std::shared_ptr<Console>	_console;
	std::shared_ptr<Menu>		_menu;
	std::shared_ptr<Board>		_board;

	const int	_boardSize = 3;
	int			_roundAmount = -1;
	int			_currentRound = 0;
	int			_draw = 0;
	int			_smartWon = 0;
};
