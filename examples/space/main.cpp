#include <iostream>

#include "BLApplication.hpp"
#include "SpaceGame.hpp"

int main(int, char**)
{
    BLApplication& app = *BLApplication::instance();
    //app.init();
    BlinkGame* game = new SpaceGame();
    app.init(game);
    if (!app.run())
        BLApplication::destroyInstance();
    return 0;
}
