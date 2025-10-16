#include "ConnectGame.hpp"

void ConnectGame::tick(float dt, std::span<const EventData> ev)
{
    model.play(-1); // process any queued commands
    model.fireWin();
    model.preview(-1); // clear preview
    for (const auto& e : ev)
    {
        if (e.type == Event::MouseButton && e.f1.value > 0.5f) // down
        {
            glm::ivec2 mp = { (int)e.f2.value, (int)e.f3.value };
            if (pointInRect(mp, gui.grid.rect))
            {
                Coordinate c = gui.grid.coordinate({ (float)mp.x, (float)mp.y });
                if (!model.gameOver)
                {
                    model.play(c.x);
                }
            }
            else if (pointInRect(mp, gui.newGameRect))
            {
                model = ConnectModel();
                gui = ConnectGui();
            }
        }
        else if (e.type == Event::MouseMove)
        {
            glm::ivec2 mp = { (int)e.f2.value, (int)e.f3.value };
            if (pointInRect(mp, gui.grid.rect))
            {
                Coordinate c = gui.grid.coordinate({ (float)mp.x, (float)mp.y });
                model.preview(c.x);
            }
            else
            {
                model.preview(-1);
            }
        }
    }
    // update render snapshot
    rs.quads.clear();
    gui.update(model);
}