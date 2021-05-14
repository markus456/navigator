#include "graphics.hh"

#include <cassert>

void Polygon::set_points(std::vector<Point> points)
{
    m_points = points;
}

void Polygon::set_color(const Color &color)
{
    m_color = color;
}

void Polygon::render(SDL_Renderer *renderer) const
{
    if (m_points.empty())
    {
        return;
    }

    SDL_SetRenderDrawColor(renderer, m_color.red, m_color.green, m_color.blue, m_color.alpha);

    auto prev = m_points.begin();
    auto it = std::next(prev);

    for (; it != m_points.end(); ++it)
    {
        SDL_RenderDrawLineF(renderer, it->x, it->y, prev->x, prev->y);
        prev = it;
    }

    it = m_points.begin();
    SDL_RenderDrawLineF(renderer, prev->x, prev->y, it->x, it->y);
}

Texture::Texture(SDL_Renderer *renderer, std::string file)
{
    SDL_Surface *data = SDL_LoadBMP(file.c_str());

    if (!data)
    {
        throw Error(SDL_GetError());
    }

    m_texture = SDL_CreateTextureFromSurface(renderer, data);
    SDL_FreeSurface(data);
}

void Texture::render(SDL_Renderer *renderer, SDL_Rect *rect, double angle) const
{
    SDL_RenderCopyEx(renderer, m_texture, nullptr, rect, angle, nullptr, SDL_FLIP_NONE);
}

Texture::~Texture()
{
    SDL_DestroyTexture(m_texture);
}
