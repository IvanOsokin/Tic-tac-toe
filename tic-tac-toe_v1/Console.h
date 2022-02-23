#pragma once

#include <Windows.h>

class Console
{
public:
	void Init();

	void SetPosition(const SHORT x, const SHORT y);
	void SetCursorVisibility(bool visible);
	void SetBackgroundColor(WORD color);
	void SetForegroundColor(WORD color);

	const HANDLE& GetWindowHandle() const { return _hStdOut; }

private:
	void SetConsoleWindow();

	HANDLE _hStdOut;
};
