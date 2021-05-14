#pragma once

#include "common.hh"
#include "objects.hh"
#include "events.hh"
#include "graphics.hh"

#include <memory>

class Navigator : public Object, public EventListener<Navigator>, public Polygon
{
public:
    Navigator();

    static std::unique_ptr<Navigator> create();

    void tick() override;

private:
    void on_mouse_move(const SDL_Event &event);

    Polygon m_renderer;
};
