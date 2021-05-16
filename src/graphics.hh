#pragma once

#include "common.hh"
#include "objects.hh"

struct Color
{
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    uint8_t alpha = 255;
};

static constexpr const Color COLOR_RED = {255, 0, 0};
static constexpr const Color COLOR_GREEN = {0, 255, 0};
static constexpr const Color COLOR_BLUE = {0, 0, 255};
static constexpr const Color COLOR_WHITE = {255, 255, 255};
static constexpr const Color COLOR_BLACK = {0, 0, 0};

// A graphical element that can be rendered
struct Renderable
{
    virtual void render(SDL_Renderer *renderer) const = 0;
};

struct Polygon : public Renderable
{
    Polygon(Object *obj, SDL_Renderer *renderer);

    ~Polygon();

    void set_fill(const Color &color);

    void set_outline(const Color &color);

    // Redraws the polygon, must be called whenever the color of the polygon changes
    void redraw();

    void render(SDL_Renderer *renderer) const override;

private:
    Object *m_obj;
    SDL_Renderer *m_renderer;
    SDL_Texture *m_texture;
    int m_width;
    int m_height;
    Color m_fill = COLOR_WHITE;
    Color m_outline = COLOR_BLACK;
};

class Texture
{
public:
    Texture &operator=(const Texture &) = delete;
    Texture(const Texture &) = delete;

    Texture(SDL_Renderer *renderer, std::string file);

    void render(SDL_Renderer *renderer, SDL_Rect *rect, double angle) const;

    ~Texture();

private:
    SDL_Texture *m_texture{nullptr};
};
