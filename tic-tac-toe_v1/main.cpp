#include "Game.h"

int main()
{
	auto game = std::make_shared<Game>();
	game->Start();

	return 0;
}