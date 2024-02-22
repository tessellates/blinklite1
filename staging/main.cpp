#include <iostream>

#include "BLApplication.hpp"

int main(int, char**)
{
    BLApplication& app = *BLApplication::instance();
    app.init();
    if (!app.run())
        BLApplication::destroyInstance();
    return 0;
}
