#include <chrono>
#include <iostream>
#include <thread>
#include <exception>
#include <vector>
#include <algorithm>

#include "common.hh"
#include "graphics.hh"
#include "objects.hh"
#include "world.hh"

using namespace std;
using chrono::duration_cast;
using chrono::microseconds;
using chrono::milliseconds;

using Clock = chrono::steady_clock;

static constexpr int WINDOW_WIDTH = 800;
static constexpr int WINDOW_HEIGHT = 600;
static constexpr int FRAMERATE = 120;

class Program
{
public:
    Program()
    {
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        {
            throw Error("SDL init failed");
        }

        m_window = SDL_CreateWindow("Navigator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);

        if (!m_window)
        {
            throw Error("Window init failed");
        }

        m_renderer = SDL_CreateRenderer(m_window, -1, 0);

        if (!m_renderer)
        {
            throw Error("Renderer init failed");
        }

        SDL_RenderGetViewport(m_renderer, &m_camera);
    }

    void poll_event()
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                m_running = false;
                break;

            case SDL_MOUSEMOTION:
                m_mouse.x = event.motion.x;
                m_mouse.y = event.motion.y;
                break;

            case SDL_MOUSEWHEEL:
                for (const auto &o : m_objects)
                {
                    if (o->is_inside(m_mouse))
                    {
                        if (event.wheel.y > 0)
                        {
                            o->set_rotation(o->rotation() + 5);
                        }
                        else if (event.wheel.y < 0)
                        {
                            o->set_rotation(o->rotation() - 5);
                        }
                    }
                }
                break;

            case SDL_KEYDOWN:
                for (const auto &o : m_objects)
                {
                    if (o->is_inside(m_mouse))
                    {
                        switch (event.key.keysym.sym)
                        {
                        case SDLK_LEFT:
                            o->set_position(o->position() + Point{-1, 0});
                            break;
                        case SDLK_RIGHT:
                            o->set_position(o->position() + Point{1, 0});
                            break;
                        case SDLK_DOWN:
                            o->set_position(o->position() + Point{0, 1});
                            break;
                        case SDLK_UP:
                            o->set_position(o->position() + Point{0, -1});
                            break;
                        }
                    }
                }

                switch (event.key.keysym.sym)
                {
                case SDLK_w:
                    m_camera.y -= 1;
                    break;
                case SDLK_s:
                    m_camera.y += 1;
                    break;
                case SDLK_a:
                    m_camera.x += 1;
                    break;
                case SDLK_d:
                    m_camera.x -= 1;
                    break;
                }

                break;

            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    m_objects.push_back(Navigator::create());
                    m_objects.back()->set_position({m_mouse.x, m_mouse.y});
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    auto it = std::remove_if(m_objects.begin(), m_objects.end(), [&](const auto &o) {
                        return o->is_inside(m_mouse);
                    });
                    m_objects.erase(it, m_objects.end());
                }
                break;

            default:
                //cout << "Got event: " << event.type << endl;
                break;
            }
        }
    }

    void render()
    {
        SDL_SetRenderDrawColor(m_renderer, 50, 50, 50, 255);
        SDL_RenderClear(m_renderer);

        for (const auto &l : m_objects)
        {
            for (auto line : l->lines())
            {
                int collisions = 0;

                for (const auto &r : m_objects)
                {
                    if (r.get() == l.get())
                    {
                        continue;
                    }

                    auto [collided, points] = r->get_collisions(line);

                    if (collided)
                    {
                        ++collisions;

                        for (auto p : points)
                        {
                            SDL_Rect rect;
                            rect.w = 10;
                            rect.h = 10;
                            rect.x = p.x - 5;
                            rect.y = p.y - 5;

                            SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
                            SDL_RenderFillRect(m_renderer, &rect);
                        }
                    }
                }

                SDL_RenderSetViewport(m_renderer, &m_camera);

                if (collisions)
                {
                    SDL_SetRenderDrawColor(m_renderer, 0, 0, 255, 255);
                }
                else
                {
                    SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);
                }

                SDL_RenderDrawLineF(m_renderer, line.first.x, line.first.y, line.second.x, line.second.y);
            }
        }

        // Texture texture(m_renderer, "media/image.bmp");
        // texture.render(m_renderer, &rect);

        SDL_RenderPresent(m_renderer);
    }

    void
    run()
    {
        constexpr const milliseconds frame_time{1000 / FRAMERATE};
        auto prev = Clock::now();
        int frames = 0;

        cout << "Framerate: " << FRAMERATE << endl;
        cout << "ms per frame: " << frame_time.count() << endl;

        while (m_running)
        {
            poll_event();
            render();

            auto now = Clock::now();
            auto loop_time = duration_cast<milliseconds>(now - prev);
            auto time_left = frame_time - loop_time;

            int frames_ms = duration_cast<milliseconds>(time_left).count();

            frames = 1000 / max(loop_time.count(), frame_time.count());
            prev = now;

            if (time_left.count() > 0)
            {
                this_thread::sleep_for(time_left);
            }

            //cout << "\rLoop time: " << loop_time.count() << " Framerate: " << frames << "                 ";
            //cout.flush();
        }
    }

    ~Program()
    {
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        SDL_Quit();
    }

private:
    SDL_Window *m_window{nullptr};
    SDL_Renderer *m_renderer{nullptr};
    SDL_Rect m_camera;
    bool m_running{true};

    Point m_mouse;

    std::unique_ptr<Object> m_next_obj;
    std::vector<std::unique_ptr<Object>> m_objects;
};

int main(int argc, char **argv)
{
    try
    {
        Program program;
        program.run();
    }
    catch (runtime_error err)
    {
        cout << err.what() << endl;
        this_thread::sleep_for(chrono::seconds(5));
    }

    return 0;
}