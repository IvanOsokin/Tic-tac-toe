#include "Console.h"

void Console::Init()
{
	_hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleWindow();
	SetCursorVisibility(false);
}

void Console::SetPosition(const SHORT x, const SHORT y)
{
	COORD coord{ x, y };
	SetConsoleCursorPosition(_hStdOut, coord);
}

void Console::SetCursorVisibility(bool visible)
{
	CONSOLE_CURSOR_INFO cursorInfo{ 1, visible };
	SetConsoleCursorInfo(_hStdOut, &cursorInfo);
}

void Console::SetBackgroundColor(WORD color)
{
	SetConsoleTextAttribute(_hStdOut, color);
}

void Console::SetForegroundColor(WORD color)
{
	SetConsoleTextAttribute(_hStdOut, color);
}

void Console::SetConsoleWindow()
{
	auto windowHandle = GetWindowHandle();
	const COORD largestWindowSize = GetLargestConsoleWindowSize(windowHandle);

	// Set left upper corner of the console
	const COORD initPosition = { 200, 200 };
	SetWindowPos(GetConsoleWindow(), HWND_TOP, initPosition.X, initPosition.Y, 300, 300, NULL);

	const COORD windowSize = { 300, 300 };
	SetConsoleScreenBufferSize(windowHandle, windowSize);

	SMALL_RECT newConsoleWindow = { 0, 0, static_cast<short>(windowSize.X - 1), static_cast<short>(windowSize.Y - 1) };
	SetConsoleWindowInfo(windowHandle, true, &newConsoleWindow);
}
