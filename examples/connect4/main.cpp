#include <iostream>

#include "BLApplication.hpp"
#include "ConnectGame.hpp"

int main(int, char**)
{
    BLApplication& app = *BLApplication::instance();
    //app.init();
    BlinkGame* game = new ConnectGame();
    app.init(game);
    if (!app.run())
        BLApplication::destroyInstance();
    return 0;
}
