#include "blockGame.h"

#include <iostream>

#define _USE_MATH_DEFINES // for C++
#include <cmath>


int main(int argc, char **argv) {
	try {
		blockGame::BlockGame blockGame = blockGame::BlockGame();

		blockGame.run();
	}
	catch (std::exception & e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}