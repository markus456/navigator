#include "graphics.hh"

#include <cassert>
#include <unordered_map>

//
// Polygon
//

Polygon::Polygon(Object *obj, SDL_Renderer *renderer)
    : m_obj(obj), m_renderer(renderer)
{
    // Add some extra space so that bounding lines are drawn correctly for rectangles
    auto [min, max] = m_obj->bounding_rect();
    m_width = (max.x - 0) + 5;
    m_height = (max.y - 0) + 5;

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

//
// Text
//

class FontLoader
{
public:
    ~FontLoader()
    {
        for (const auto &kv : m_fonts)
        {
            TTF_CloseFont(kv.second);
        }
    }

    TTF_Font *load(std::string filename, int size)
    {
        std::string font_name = filename + '-' + std::to_string(size);
        auto it = m_fonts.find(font_name);

        if (it == m_fonts.end())
        {
            auto font = TTF_OpenFont(filename.c_str(), size);

            if (font)
            {
                it = m_fonts.emplace(font_name, font).first;
            }
            else
            {
                throw Error("Could not load font " + font_name + ": " + std::string(TTF_GetError()));
            }
        }

        return it->second;
    }

private:
    std::unordered_map<std::string, TTF_Font *> m_fonts;
};

static std::unique_ptr<FontLoader> loader;

// static
void Text::init()
{
    if (TTF_Init() != 0)
    {
        throw Error("TTF_Init failed");
    }

    loader = std::make_unique<FontLoader>();
}

void Text::finish()
{
    loader.reset();
    TTF_Quit();
}

Text::Text(SDL_Renderer *renderer)
    : m_renderer(renderer)
{
}

void Text::set_text(std::string text, std::string font, Color color, int size)
{
    SDL_Color sdl_color = {color.red, color.green, color.blue, color.alpha};
    SDL_Surface *surface = TTF_RenderText_Solid(loader->load(font, size),
                                                text.c_str(), sdl_color);
    m_texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    m_rect.w = surface->w;
    m_rect.h = surface->h;

    SDL_FreeSurface(surface);
}

void Text::set_position(Point point)
{
    m_rect.x = point.x;
    m_rect.y = point.y;
}

void Text::render(SDL_Renderer *renderer) const
{
    SDL_RenderCopyEx(renderer, m_texture, nullptr, &m_rect, 0, nullptr, SDL_FLIP_NONE);
}

Text::~Text()
{
    SDL_DestroyTexture(m_texture);
}
