#include "header.h"
int main() {
    system("color FC");
    using namespace WordGame;
    srand(static_cast<unsigned int>(time(0)));
    Game game;
    game.displayMenu();
    return 0;
}