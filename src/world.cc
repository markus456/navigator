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
    set_position(position() + m_motion);
    set_rotation(rotation() + m_rotation);

    if (collision())
    {
        set_position(position() - m_motion);
        set_rotation(rotation() - m_rotation);
        m_motion.x = 0;
        m_motion.y = 0;
        m_rotation = 0;
    }
}

Color Navigator::fill_color() const
{
    if (!is_active())
    {
        return COLOR_GRAY;
    }
    else if (!is_collision_enabled())
    {
        return COLOR_MAGENTA;
    }
    else if (m_selected)
    {
        return COLOR_BLUE;
    }
    else
    {
        return COLOR_GREEN;
    }
}

Color Navigator::outline_color() const
{
    if (m_hover || m_selected)
    {
        return COLOR_RED;
    }
    else
    {
        return COLOR_BLACK;
    }
}

void Navigator::state_changed(Object::ChangeType type)
{
    switch (type)
    {
    case Object::ChangeType::COLLISION:
    case Object::ChangeType::ACTIVE:
        m_polygon.set_fill(fill_color());
        m_polygon.set_outline(outline_color());
        m_polygon.redraw();
        break;
    }
}

void Navigator::render(SDL_Renderer *renderer) const
{
    m_polygon.render(renderer);
}

void Navigator::on_mouse_move(const SDL_Event &event)
{
    Point mouse{(double)event.motion.x, (double)event.motion.y};
    bool hover = is_inside(mouse);

    if (m_hover != hover)
    {
        m_hover = hover;

        m_polygon.set_outline(outline_color());
        m_polygon.redraw();
    }
}

void Navigator::on_key_up(const SDL_Event &event)
{
    switch (event.key.keysym.sym)
    {
    case SDLK_a:
    case SDLK_d:
        m_motion.x = 0;
        break;

    case SDLK_w:
    case SDLK_s:
        m_motion.y = 0;
        break;

    case SDLK_q:
    case SDLK_e:
        m_rotation = 0;
        break;
    }
}

void Navigator::on_key_down(const SDL_Event &event)
{
    double speed = (SDL_GetModState() & KMOD_SHIFT) ? 0.1 : 1.0;

    switch (event.key.keysym.sym)
    {
    case SDLK_a:
        m_motion.x = -speed;
        break;

    case SDLK_d:
        m_motion.x = speed;
        break;

    case SDLK_w:
        m_motion.y = -speed;
        break;

    case SDLK_s:
        m_motion.y = speed;
        break;

    case SDLK_q:
        m_rotation = speed;
        break;

    case SDLK_e:
        m_rotation = -speed;
        break;
    }
}

void Navigator::set_selected(bool selected)
{
    m_selected = selected;
    m_polygon.set_fill(fill_color());
    m_polygon.set_outline(outline_color());
    m_polygon.redraw();

    if (selected)
    {
        listen(SDL_KEYDOWN, &Navigator::on_key_down);
        listen(SDL_KEYUP, &Navigator::on_key_up);
    }
    else
    {
        stop_listening(SDL_KEYDOWN);
        stop_listening(SDL_KEYUP);
    }
}
