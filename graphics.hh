#pragma once

#include "common.hh"

struct Renderable
{
    virtual void render(SDL_Renderer *renderer) const = 0;
};

class Texture
{
public:
    Texture &operator=(const Texture &) = delete;
    Texture(const Texture &) = delete;

    Texture(SDL_Renderer *renderer, std::string file)
    {
        SDL_Surface *data = SDL_LoadBMP(file.c_str());

        if (!data)
        {
            throw Error(SDL_GetError());
        }

        m_texture = SDL_CreateTextureFromSurface(renderer, data);
        SDL_FreeSurface(data);
    }

    void render(SDL_Renderer *renderer, SDL_Rect *rect, double angle) const
    {
        SDL_RenderCopyEx(renderer, m_texture, nullptr, rect, angle, nullptr, SDL_FLIP_NONE);
    }

    ~Texture()
    {
        SDL_DestroyTexture(m_texture);
    }

private:
    SDL_Texture *m_texture{nullptr};
};
