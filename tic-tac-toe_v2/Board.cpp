#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <algorithm>
#include <thread>
#include <chrono>
#include <ctime>

#include "Game.h"
#include "Board.h"
#include "Console.h"
#include "Player.h"

Board::~Board()
{
	for (int i = 0; i < _size; ++i)
	{
		delete[] _board[i];
	}

	delete[] _board;
}

void Board::PreInit(std::shared_ptr<Console> console, int boardSize)
{
	_console = console;
	_size = boardSize * 2 - 1;

	CreateBoard();
	SetCellSize();
}

void Board::Init()
{
	CleanBoard();

	InitPlayers();

	PrintInfo();

	ResetFreeCellsVector();
}

void Board::Update()
{
	if (TryPutChar())
	{
		TryFindWinner();
	}

	static const auto numberLinesToExcept = static_cast<std::int64_t>(3) + _size;
	if (!HasFreeCell() || _lineExceptions.size() == numberLinesToExcept)
	{
		_shouldClose = true;
		return;
	}

	if (!_shouldClose)
	{
		SwapPlayers();
	}
}

void Board::Render()
{
	DrawBoard();

	if (_winLine != WinLine::noWinner)
	{
		HighlightWinner();
	}
}

void Board::Reset()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	ShouldClose(false);

	_info.clear();
	_lineExceptions.clear();

	if (_players._active._smart)
	{
		_players._active.ResetSmart();
	}
	else
	{
		_players._notActive.ResetSmart();
	}

	_winLine = WinLine::noWinner;
}

void Board::InitPlayers()
{
	std::srand(static_cast<unsigned>(std::time(nullptr)));

	bool firstMove = std::rand() % 2;

	if (firstMove)
	{
		_players._active.Init('x', PlayerType::randomMove);
		_players._notActive.Init('o', PlayerType::smartMove);

		_info.emplace_back("Player \'x\' makes \"random\" moves");
		_info.emplace_back("Player \'o\' makes \"smart\" moves");
	}
	else
	{
		_players._active.Init('x', PlayerType::smartMove);
		_players._notActive.Init('o', PlayerType::randomMove);

		_info.emplace_back("Player \'x\' makes \"smart\" moves");
		_info.emplace_back("Player \'o\' makes \"random\" moves");
	}
}

void Board::SwapPlayers()
{
	std::swap(_players._active, _players._notActive);
}

bool Board::TryPutChar()
{
	COORD coord{ 0, 0 };

	if (IsBoardClear())
	{
		FirstMove(coord);
	}

	if (_players._active._smart)
	{
		do
		{
			if (_lineExceptions.size() == static_cast <std::int64_t>(3) + _size)
			{
				_shouldClose = true;
				return false;
			}
			_players._active.Reset();
			_players._notActive.Reset();

			FindSuitableCellToPut(coord);
		}while (!CanPutHere(coord));
	}
	else
	{
		MoveRandomly(coord);
	}

	_cursor._x = coord.X;
	_cursor._y = coord.Y;

	std::this_thread::sleep_for(std::chrono::milliseconds(5));

	_board[_cursor._y][_cursor._x] = _players._active.GetChar();

	ExpelCell(coord);

	return true;
}

void Board::FirstMove(COORD& coord)
{
	std::srand(static_cast<unsigned>(std::time(nullptr)));

	coord.X = std::rand() % (_size - 1);
	coord.Y = std::rand() % (_size - 1);

	if (coord.X % 2 == 1)	++coord.X;
	if (coord.Y % 2 == 1)	++coord.Y;
}

void Board::MoveRandomly(COORD& coord)
{
	do
	{
		coord = FindInRestCells();
	}while(!CanPutHere(coord));
}

void Board::ExpelCell(const COORD& coord)
{
	auto removeCell = std::remove_if(_freeCells.begin(), _freeCells.end(), [&coord](const COORD& _coord)
	{
		if (coord.X == _coord.X && coord.Y == _coord.Y)
		{
			return true;
		}

		return false;
	});

	_freeCells.erase(removeCell, _freeCells.end());
}

COORD Board::FindInRestCells()
{
	std::srand(static_cast<unsigned>(std::time(nullptr)));
	int index = std::rand() % _freeCells.size();

	return _freeCells[index];
}

void Board::TryFindWinner()
{
	char currentSymbol = _players._active.GetChar();

	//Check the main diagonal
	for (int i = 0; i < _size; i += 2)
	{
		if (_board[i][i] != currentSymbol)
		{
			_winner = false;
			break;
		}
	}
	if (_winner)
	{
		_winLine = WinLine::mainDiagonal;
		_shouldClose = true;
		return;
	}
	_winner = true;

	//Check the auxiliary diagonal
	for (int i = _size - 1; i >= 0; i -= 2)
	{
		const auto j = _size - i - 1;
		if (_board[i][j] != currentSymbol)
		{
			_winner = false;
			break;
		}
	}
	if (_winner)
	{
		_winLine = WinLine::auxDiagonal;
		_shouldClose = true;
		return;
	}
	_winner = true;

	//Check the vertical
	for (int i = 0; i < _size; i += 2)
	{
		for (int j = 0; j < _size; j += 2)
		{
			if (_board[j][i] != currentSymbol)
			{
				_winner = false;
				break;
			}
		}
		if (_winner)
		{
			_winLine = WinLine::vertical;
			_shouldClose = true;
			return;
		}
		_winner = true;
	}

	//Check the horizontal
	for (int i = 0; i < _size; i += 2)
	{
		for (int j = 0; j < _size; j += 2)
		{
			if (_board[i][j] != currentSymbol)
			{
				_winner = false;
				break;
			}
		}
		if (_winner)
		{
			_winLine = WinLine::horizontal;
			_shouldClose = true;
			return;
		}
		_winner = true;
	}
}

bool Board::HasFreeCell()
{
	for (int i = 0; i < _size; ++i)
	{
		for (int j = 0; j < _size; ++j)
		{
			if (_board[i][j] == ' ')
			{
				return true;
			}
		}
	}

	return false;
}

void Board::FindSuitableCellToPut(COORD& coord)
{
	FindInRows(coord);
	FindInColumns(coord);
	FindInMainDiagonal(coord);
	FindInAuxDiagonal(coord);

	FindFreeCell(WhereShouldToPut(), coord);

	_shouldExceptLine = false;
}

void Board::FindInRows(COORD& coord)
{
	for (int i = 0; i < _size; i += 2)
	{
		bool skipStep = false;

		for (const auto& shouldExcept : _lineExceptions)
		{
			if (shouldExcept.first == SmartPlayer::WhereMaxAmount::row && shouldExcept.second == i)
			{
				skipStep = true;
				break;
			}
		}

		if (skipStep)
		{
			continue;
		}

		int currenMaxAmountOwnChar = 0;
		int currenMaxAmountEnemyChar = 0;

		for (int j = 0; j < _size; j += 2)
		{
			if (_board[i][j] == _players._active.GetChar())
			{
				++currenMaxAmountOwnChar;
			}
			if (_board[i][j] == _players._notActive.GetChar())
			{
				++currenMaxAmountEnemyChar;
			}
		}

		if (currenMaxAmountOwnChar != 0 && currenMaxAmountEnemyChar != 0)
		{
			_lineExceptions.insert(std::make_pair(SmartPlayer::WhereMaxAmount::row, i));
			continue;
		}

		if (currenMaxAmountOwnChar != 0 || currenMaxAmountEnemyChar != 0)
		{
			FindMaxAmount(currenMaxAmountOwnChar, currenMaxAmountEnemyChar, i, SmartPlayer::WhereMaxAmount::row);
			continue;
		}

		coord.X = std::rand() % (_size - 1);
		coord.Y = i;

		if (coord.X % 2 == 1)	++coord.X;
	}
}

void Board::FindInColumns(COORD& coord)
{
	for (int i = 0; i < _size; i += 2)
	{
		bool skipStep = false;

		for (const auto& shouldExcept : _lineExceptions)
		{
			if (shouldExcept.first == SmartPlayer::WhereMaxAmount::column && shouldExcept.second == i)
			{
				skipStep = true;
				break;
			}
		}

		if (skipStep)
		{
			continue;
		}

		int currenMaxAmountOwnChar = 0;
		int currenMaxAmountEnemyChar = 0;

		for (int j = 0; j < _size; j += 2)
		{
			if (_board[j][i] == _players._active.GetChar())
			{
				++currenMaxAmountOwnChar;
			}
			if (_board[j][i] == _players._notActive.GetChar())
			{
				++currenMaxAmountEnemyChar;
			}
		}

		if (currenMaxAmountOwnChar != 0 && currenMaxAmountEnemyChar != 0)
		{
			_lineExceptions.insert(std::make_pair(SmartPlayer::WhereMaxAmount::column, i));
			continue;
		}

		if (currenMaxAmountOwnChar != 0 || currenMaxAmountEnemyChar != 0)
		{
			FindMaxAmount(currenMaxAmountOwnChar, currenMaxAmountEnemyChar, i, SmartPlayer::WhereMaxAmount::column);
			continue;
		}

		coord.X = i;
		coord.Y = std::rand() % (_size - 1);

		if (coord.Y % 2 == 1)	++coord.Y;
	}
}

void Board::FindInMainDiagonal(COORD& coord)
{
	for (const auto& shouldExcept : _lineExceptions)
	{
		if (shouldExcept.first == SmartPlayer::WhereMaxAmount::mainDiagonal)
		{
			return;
		}
	}

	int currenMaxAmountOwnChar = 0;
	int currenMaxAmountEnemyChar = 0;

	for (int i = 0; i < _size; i += 2)
	{
		if (_board[i][i] == _players._active.GetChar())
		{
			++currenMaxAmountOwnChar;
		}
		if (_board[i][i] == _players._notActive.GetChar())
		{
			++currenMaxAmountEnemyChar;
		}
	}

	if (currenMaxAmountOwnChar != 0 && currenMaxAmountEnemyChar != 0)
	{
		_lineExceptions.insert(std::make_pair(SmartPlayer::WhereMaxAmount::mainDiagonal, 0));
		return;
	}

	if (currenMaxAmountOwnChar != 0 || currenMaxAmountEnemyChar != 0)
	{
		FindMaxAmount(currenMaxAmountOwnChar, currenMaxAmountEnemyChar, 0, SmartPlayer::WhereMaxAmount::mainDiagonal);
	}
}

void Board::FindInAuxDiagonal(COORD& coord)
{
	for (const auto& shouldExcept : _lineExceptions)
	{
		if (shouldExcept.first == SmartPlayer::WhereMaxAmount::auxDiagonal)
		{
			return;
		}
	}

	int currenMaxAmountOwnChar = 0;
	int currenMaxAmountEnemyChar = 0;

	for (int i = 0; i < _size; i += 2)
	{
		const auto j = (_size - 1) - i;

		if (_board[i][j] == _players._active.GetChar())
		{
			++currenMaxAmountOwnChar;
		}
		if (_board[i][j] == _players._notActive.GetChar())
		{
			++currenMaxAmountEnemyChar;
		}
	}

	if (currenMaxAmountOwnChar != 0 && currenMaxAmountEnemyChar != 0)
	{
		_lineExceptions.insert(std::make_pair(SmartPlayer::WhereMaxAmount::auxDiagonal, 0));
		return;
	}

	if (currenMaxAmountOwnChar != 0 || currenMaxAmountEnemyChar != 0)
	{
		FindMaxAmount(currenMaxAmountOwnChar, currenMaxAmountEnemyChar, 0, SmartPlayer::WhereMaxAmount::auxDiagonal);
	}
}

void Board::FindMaxAmount(int currentMaxOwn, int currenMaxEnemy, int lineNumber, SmartPlayer::WhereMaxAmount whereMax)
{
	if (currentMaxOwn != 0 && currenMaxEnemy != 0)
	{
		_shouldExceptLine = true;
	}

	if (currentMaxOwn > _players._active._smart->_own._howMuch)
	{
		_players._active._smart->_own._howMuch = currentMaxOwn;
		_players._active._smart->_own._where.first = whereMax;
		_players._active._smart->_own._where.second = lineNumber;
	}

	if (currenMaxEnemy > _players._active._smart->_enemy._howMuch)
	{
		_players._active._smart->_enemy._howMuch = currenMaxEnemy;
		_players._active._smart->_enemy._where.first = whereMax;
		_players._active._smart->_enemy._where.second = lineNumber;
	}
}

std::pair<SmartPlayer::WhereMaxAmount, lineNumber> Board::WhereShouldToPut()
{
	if (_players._active._smart->_own._howMuch >= _players._active._smart->_enemy._howMuch)
	{
		return _players._active._smart->_own._where;
	}
	else
	{
		return _players._active._smart->_enemy._where;
	}
}

void Board::FindFreeCell(std::pair<SmartPlayer::WhereMaxAmount, lineNumber> lineToPut, COORD& coord)
{
	if (lineToPut.first == SmartPlayer::WhereMaxAmount::row)
	{
		for (int i = 0; i < _size; i += 2)
		{
			if (_board[lineToPut.second][i] == ' ')
			{
				coord.X = static_cast<SHORT>(i);
				coord.Y = static_cast<SHORT>(lineToPut.second);

				if (_shouldExceptLine)
				{
					_lineExceptions.insert(lineToPut);

					_players._active.Reset();
				}

				return;
			}
		}
	}
	
	if (lineToPut.first == SmartPlayer::WhereMaxAmount::column)
	{
		for (int i = 0; i < _size; i += 2)
		{
			if (_board[i][lineToPut.second] == ' ')
			{
				coord.X = static_cast<SHORT>(lineToPut.second);
				coord.Y = static_cast<SHORT>(i);

				if (_shouldExceptLine)
				{
					_lineExceptions.insert(lineToPut);

					_players._active.Reset();
				}

				return;
			}
		}
	}

	if (lineToPut.first == SmartPlayer::WhereMaxAmount::mainDiagonal)
	{
		for (int i = 0; i < _size; i += 2)
		{
			if (_board[i][i] == ' ')
			{
				coord.X = static_cast<SHORT>(i);
				coord.Y = static_cast<SHORT>(i);

				if (_shouldExceptLine)
				{
					_lineExceptions.insert(lineToPut);

					_players._active.Reset();
				}

				return;
			}
		}
	}

	if (lineToPut.first == SmartPlayer::WhereMaxAmount::auxDiagonal)
	{
		for (int i = 0; i < _size; i += 2)
		{
			const auto j = (_size - 1) - i;

			if (_board[i][j] == ' ')
			{
				coord.X = static_cast<SHORT>(j);
				coord.Y = static_cast<SHORT>(i);

				if (_shouldExceptLine)
				{
					_lineExceptions.insert(lineToPut);

					_players._active.Reset();
				}

				return;
			}
		}
	}
}

bool Board::IsBoardClear()
{
	for (int i = 0; i < _size; i += 2)
	{
		for (int j = 0; j < _size; j += 2)
		{
			if (_board[i][j] != ' ')
			{
				return false;
			}
		}
	}

	return true;
}

bool Board::CanPutHere(const COORD& coord)
{
	if (_board[coord.Y][coord.X] == ' ')
	{
		return true;
	}
	
	return false;
}

void Board::ResetFreeCellsVector()
{
	_freeCells.emplace_back(COORD(0, 0));
	_freeCells.emplace_back(COORD(0, 2));
	_freeCells.emplace_back(COORD(0, 4));
	_freeCells.emplace_back(COORD(2, 0));
	_freeCells.emplace_back(COORD(2, 2));
	_freeCells.emplace_back(COORD(2, 4));
	_freeCells.emplace_back(COORD(4, 0));
	_freeCells.emplace_back(COORD(4, 2));
	_freeCells.emplace_back(COORD(4, 4));
}

void Board::CreateBoard()
{
	_board = new char* [_size];
	for (int i = 0; i < _size; ++i)
	{
		_board[i] = new char[_size];
	}
}

void Board::SetCellSize()
{
	CONSOLE_FONT_INFOEX fontInfo;
	fontInfo.cbSize = sizeof(fontInfo);
	fontInfo.nFont = 0;
	fontInfo.dwFontSize = { 25, 25 };
	fontInfo.FontFamily = FF_DONTCARE;
	fontInfo.FontWeight = 700;

	auto windowHandle = _console->GetWindowHandle();

	wcscpy(fontInfo.FaceName, L"Lucida Console");
	SetCurrentConsoleFontEx(windowHandle, false, &fontInfo);
}

void Board::CleanBoard()
{
	for (int i = 0; i < _size; ++i)
	{
		for (int j = 0; j < _size; ++j)
		{
			if (i % 2 == 1)
			{
				_board[i][j] = '-';
			}
			else if (j % 2 == 1)
			{
				_board[i][j] = '|';
			}
			else
			{
				_board[i][j] = ' ';
			}
		}
	}
}

void Board::DrawBoard()
{
	_console->SetPosition(0, 0);

	for (int i = 0; i < _size; ++i)
	{
		for (int j = 0; j < _size; ++j)
		{
			std::cout << _board[i][j];
		}
		std::cout << std::endl;
	}
}

void Board::PrintInfo()
{
	_console->SetPosition(10, 0);
	std::cout << _info[0];
	_console->SetPosition(10, 1);
	std::cout << _info[1];
}

void Board::HighlightWinner()
{
	_console->SetBackgroundColor(BackgroundColor::black);
	_console->SetForegroundColor(ForegroundColor::green);

	switch (_winLine)
	{
	case WinLine::mainDiagonal:
		for (int i = 0; i < _size; i += 2)
		{
			_console->SetPosition(i, i);
			std::cout << _board[i][i];
		}
		break;
	case WinLine::auxDiagonal:
		for (int i = 0; i < _size; i += 2)
		{
			auto j = _size - i - 1;
			_console->SetPosition(i, j);
			std::cout << _board[i][j];
		}
		break;
	case WinLine::vertical:
		for (int j = 0; j < _size; j += 2)
		{
			_console->SetPosition(_cursor._x, j);
			std::cout << _board[j][_cursor._x];
		}
		break;
	case WinLine::horizontal:
		for (int i = 0; i < _size; i += 2)
		{
			_console->SetPosition(i, _cursor._y);
			std::cout << _board[_cursor._y][i];
		}
		break;
	default:
		break;
	}

	_console->SetPosition(0, _size + 1);
	_console->SetForegroundColor(ForegroundColor::white);
}