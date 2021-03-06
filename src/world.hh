#pragma once

#include "common.hh"
#include "objects.hh"
#include "events.hh"
#include "graphics.hh"

#include <memory>

class Wall : public Object, public Renderable
{
public:
    static std::unique_ptr<Wall> create(SDL_Renderer *renderer, std::vector<Point> outline);

    ~Wall() = default;

    void tick() override;

    void state_changed(Object::ChangeType type) override;

    void render(SDL_Renderer *renderer) const override;

private:
    Wall(SDL_Renderer *renderer, std::vector<Point> outline);
    Polygon m_polygon;
};

class Navigator : public Object, public EventListener<Navigator>, public Renderable
{
public:
    static std::unique_ptr<Navigator> create(SDL_Renderer *renderer);

    void tick() override;

    void state_changed(Object::ChangeType type) override;

    void render(SDL_Renderer *renderer) const override;

    void set_selected(bool is_selected);

private:
    Navigator(SDL_Renderer *renderer);
    Navigator(SDL_Renderer *renderer, std::vector<Point> outline);

    void on_mouse_move(const SDL_Event &event);
    void on_key_down(const SDL_Event &event);
    void on_key_up(const SDL_Event &event);

    Color fill_color() const;

    Color outline_color() const;

    Polygon m_polygon;
    Point m_motion{0, 0};
    double m_rotation = 0;
    bool m_selected = false;
    bool m_hover = false;
};
