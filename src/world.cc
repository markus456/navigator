#include <tuple>
#include <cstdint>

#include "world.hh"

using namespace std;

Navigator::Navigator()
    : Object({
          {-25.0, -25.0},
          {25.0, -25.0},
          {25.0, 25.0},
          {-25.0, 25.0},
      })
{
    listen(SDL_MOUSEMOTION, &Navigator::on_mouse_move);
    set_color(COLOR_GREEN);
}

// static
std::unique_ptr<Navigator> Navigator::create()
{
    return std::make_unique<Navigator>();
}

void Navigator::tick()
{
    set_points(points());
}

void Navigator::on_mouse_move(const SDL_Event &event)
{
    Point mouse{(double)event.motion.x, (double)event.motion.y};
    set_color(is_inside(mouse) ? COLOR_BLUE : COLOR_GREEN);
}