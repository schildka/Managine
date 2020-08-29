#include "blockGame.h"

#include <iostream>

#define _USE_MATH_DEFINES // for C++
#include <cmath>


int main(int argc, char **argv) {

	blockGame::BlockGame blockGame = blockGame::BlockGame();
	
	blockGame.run();

	return 0;
}