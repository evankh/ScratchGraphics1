#include "Game.h"
#include "ServiceLocator.h"

int main(int argc, char* argv[]) {
	Game::getInstance().init(argc, argv);
	Game::getInstance().load();
	Game::getInstance().run();
	ServiceLocator::getLoggingService().log("After the main loop");
	system("pause");
	return 0;
}