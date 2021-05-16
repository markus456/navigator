#include <tuple>
#include <cstdint>

#include "world.hh"

using namespace std;

Navigator::Navigator(SDL_Renderer *renderer)
    : Object({
          {0, 0},
          {50, 0},
          {50, 50},
          {0, 50},
      }),
      m_polygon(this, renderer)
{
    listen(SDL_MOUSEMOTION, &Navigator::on_mouse_move);
    m_polygon.set_fill(COLOR_GREEN);
    m_polygon.set_outline(COLOR_BLACK);
    m_polygon.redraw();
}

// static
std::unique_ptr<Navigator> Navigator::create(SDL_Renderer *renderer)
{
    return std::make_unique<Navigator>(renderer);
}

void Navigator::tick()
{
}

void Navigator::render(SDL_Renderer *renderer) const
{
    m_polygon.render(renderer);
}

void Navigator::on_mouse_move(const SDL_Event &event)
{
    Point mouse{(double)event.motion.x, (double)event.motion.y};
    m_polygon.set_fill(is_inside(mouse) ? COLOR_BLUE : COLOR_GREEN);
    m_polygon.redraw();
}