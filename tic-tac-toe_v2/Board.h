#pragma once

#include <Windows.h>
#include <vector>
#include <map>

#include "Player.h"

enum WinLine
{
	noWinner,
	vertical,
	horizontal,
	mainDiagonal,
	auxDiagonal,
};

struct SmartPlayer;

class Console;
class Player;

class Board
{
public:
	~Board();

	void PreInit(std::shared_ptr<Console> console, int boardSize);
	void Init();

	void Update();
	void Render();
	void Reset();

	bool ShouldClose() const { return _shouldClose; }
	WinLine GetWinner() const { return _winLine; }

private:
	void InitPlayers();

	void SwapPlayers();
	bool TryPutChar();
	void FirstMove(COORD& coord);
	void MoveRandomly(COORD& coord);
	void ExpelCell(const COORD& coord);
	COORD FindInRestCells();
	void TryFindWinner();
	bool HasFreeCell();
	void FindSuitableCellToPut(COORD& coord);
	void FindInRows(COORD& coord);
	void FindInColumns(COORD& coord);
	void FindInMainDiagonal(COORD& coord);
	void FindInAuxDiagonal(COORD& coord);
	void FindMaxAmount(int currentMax, int currenMaxEnemy, int lineNumber, SmartPlayer::WhereMaxAmount whereMax);
	std::pair<SmartPlayer::WhereMaxAmount, lineNumber> WhereShouldToPut();
	void FindFreeCell(std::pair<SmartPlayer::WhereMaxAmount, lineNumber> lineToPut, COORD& coord);
	bool IsBoardClear();
	bool CanPutHere(const COORD& coord);
	void ResetFreeCellsVector();

	void CreateBoard();
	void SetCellSize();
	void CleanBoard();
	void ShouldClose(bool shouldClose) { _shouldClose = shouldClose; }

	void DrawBoard();
	void PrintInfo();
	void HighlightWinner();

	enum BackgroundColor
	{
		black = 0,
		grey = BACKGROUND_INTENSITY,
	};

	enum ForegroundColor
	{
		white = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		green = FOREGROUND_GREEN,
	};

	struct Cursor
	{
		int _x = 0;
		int _y = 0;
		BackgroundColor _color = black;
	};

	struct Players
	{
		Players() : _active(), _notActive() {};

		Player _active;
		Player _notActive;
	};

	std::multimap<SmartPlayer::WhereMaxAmount, lineNumber>	_lineExceptions;
	std::vector<std::string>								_info;
	std::vector<COORD>										_freeCells;

	std::shared_ptr<Console> _console;

	WinLine		_winLine;
	Cursor		_cursor;

	Players	_players;

	int		_size = 0;
	char**	_board = nullptr;
	bool	_winner = true;
	bool	_shouldClose = false;
	bool	_shouldExceptLine = false;
};