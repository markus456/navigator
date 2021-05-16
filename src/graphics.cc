#include "graphics.hh"

#include <cassert>

//
// Polygon
//

Polygon::Polygon(Object *obj, SDL_Renderer *renderer)
    : m_obj(obj), m_renderer(renderer)
{
    // Add some extra space so that bounding lines are drawn correctly for rectangles
    auto [min, max] = m_obj->bounding_rect();
    m_width = (max.x - min.x) + 5;
    m_height = (max.y - min.y) + 5;

    m_texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, m_width, m_height);
}

Polygon::~Polygon()
{
    SDL_DestroyTexture(m_texture);
}

void Polygon::redraw()
{
    SDL_SetRenderTarget(m_renderer, m_texture);

    // Set blendmode so that alpha blending works correctly (by default it doesn't).
    SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
    SDL_Rect rect{0, 0, m_width, m_height};
    SDL_RenderFillRect(m_renderer, &rect);

    SDL_SetRenderDrawColor(m_renderer, m_fill.red, m_fill.green, m_fill.blue, m_fill.alpha);

    for (const auto &line : m_obj->scan_lines())
    {
        SDL_RenderDrawLineF(m_renderer, line.first.x, line.first.y, line.second.x, line.second.y);
    }

    SDL_SetRenderDrawColor(m_renderer, m_outline.red, m_outline.green, m_outline.blue, m_outline.alpha);

    for (const auto &line : m_obj->bounding_lines())
    {
        SDL_RenderDrawLineF(m_renderer, line.first.x, line.first.y, line.second.x, line.second.y);
    }

    SDL_SetRenderTarget(m_renderer, nullptr);
}

void Polygon::set_fill(const Color &color)
{
    m_fill = color;
}

void Polygon::set_outline(const Color &color)
{
    m_outline = color;
}

void Polygon::render(SDL_Renderer *renderer) const
{
    auto pos = m_obj->position();
    SDL_Rect dstrect{(int)pos.x, (int)pos.y, m_width, m_height};
    SDL_Point rot{(int)m_obj->center().x, (int)m_obj->center().y};

    SDL_RenderCopyEx(renderer, m_texture, nullptr, &dstrect, m_obj->rotation(), &rot, SDL_FLIP_NONE);
}

//
// Texture
//

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
    SDL_Point rot{0, 0};
    SDL_RenderCopyEx(renderer, m_texture, nullptr, rect, angle, &rot, SDL_FLIP_NONE);
}

Texture::~Texture()
{
    SDL_DestroyTexture(m_texture);
}
