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
    void set_points(std::vector<Point> points);

    void set_color(const Color &color);

    void render(SDL_Renderer *renderer) const override;

private:
    std::vector<Point> m_points;
    Color m_color;
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
