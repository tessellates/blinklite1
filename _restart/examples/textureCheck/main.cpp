#include <iostream>

#include "BLApplication.hpp"
#include "CheckGame.hpp"

int main(int, char**)
{
    BLApplication& app = *BLApplication::instance();
    //app.init();
    BlinkGame* game = new CheckGame();
    app.init(game);
    if (!app.run())
        BLApplication::destroyInstance();
    return 0;
}
