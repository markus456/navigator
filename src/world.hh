#pragma once

#include "common.hh"
#include "objects.hh"
#include "events.hh"
#include "graphics.hh"

#include <memory>

class Navigator : public Object, public EventListener<Navigator>, public Renderable
{
public:
    Navigator(SDL_Renderer *renderer);

    static std::unique_ptr<Navigator> create(SDL_Renderer *renderer);

    void tick() override;

    void render(SDL_Renderer *renderer) const override;

private:
    void on_mouse_move(const SDL_Event &event);

    Polygon m_polygon;
};
