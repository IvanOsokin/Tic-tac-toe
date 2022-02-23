#pragma once

#include <Windows.h>

struct Position {
	int x, y;
};

struct Color {
	long long color;
};

class Console
{
public:
	void Init();

	void SetConsoleParam();
	void SetPosition(const SHORT x, const SHORT y);
	void SetCursorVisibility(bool visible);
	void SetBackgroundColor(WORD color);
	void SetForegroundColor(WORD color);
	void CleanConsole();

	const HANDLE& GetWindowHandle() const { return _hStdOut; }

private:
	void SetConsoleWindow();

	HANDLE _hStdOut;
};
