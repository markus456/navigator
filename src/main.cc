#include <chrono>
#include <iostream>
#include <thread>
#include <exception>
#include <vector>
#include <algorithm>
#include <sstream>

#include "common.hh"
#include "graphics.hh"
#include "objects.hh"
#include "world.hh"
#include "events.hh"

using namespace std;
using chrono::duration_cast;
using chrono::microseconds;
using chrono::milliseconds;

using Clock = chrono::steady_clock;

static constexpr int WINDOW_WIDTH = 800;
static constexpr int WINDOW_HEIGHT = 600;
static constexpr int FRAMERATE = 120;

static const std::string FONT_NAME = "fonts/pixeldroidMenuRegular.ttf";
static const Color FONT_COLOR = COLOR_WHITE;
static const int FONT_SIZE = 25;

class Program
{
public:
    Program()
    {
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        {
            throw Error("SDL init failed");
        }

        Text::init();

        m_window = SDL_CreateWindow("Navigator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);

        if (!m_window)
        {
            throw Error("Window init failed");
        }

        m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);

        if (!m_renderer)
        {
            throw Error("Renderer init failed");
        }

        m_mouse_label = std::make_unique<Text>(m_renderer);

        SDL_RenderGetViewport(m_renderer, &m_camera);

        EventGenerator::add(this, SDL_QUIT, [this](const auto &event)
                            { m_running = false; });

        EventGenerator::add(this, SDL_MOUSEMOTION, [this](const auto &event)
                            { on_mouse_move(event); });

        EventGenerator::add(this, SDL_MOUSEWHEEL, [this](const auto &event)
                            { on_mouse_wheel(event); });

        EventGenerator::add(this, SDL_KEYDOWN, [this](const auto &event)
                            { on_keydown(event); });

        EventGenerator::add(this, SDL_MOUSEBUTTONUP, [this](const auto &event)
                            { on_mousebuttonup(event); });
    }

    ~Program()
    {
        SDL_DestroyRenderer(m_renderer);
        SDL_DestroyWindow(m_window);
        Text::finish();
        SDL_Quit();
    }

    void on_mouse_move(const SDL_Event &event)
    {
        m_mouse.x = event.motion.x;
        m_mouse.y = event.motion.y;

        std::ostringstream ss;
        ss << "X: " << m_mouse.x << " Y: " << m_mouse.y;

        m_mouse_label->set_text(ss.str(), FONT_NAME, FONT_COLOR, FONT_SIZE);
        auto p = m_mouse;
        p += Point(0, -20);
        m_mouse_label->set_position(p);
    }

    void on_mouse_wheel(const SDL_Event &event)
    {
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
    }

    void on_keydown(const SDL_Event &event)
    {
        switch (event.key.keysym.sym)
        {
        case SDLK_LEFT:
            m_camera.x -= 1;
            break;
        case SDLK_RIGHT:
            m_camera.x += 1;
            break;
        case SDLK_UP:
            m_camera.y += 1;
            break;
        case SDLK_DOWN:
            m_camera.y -= 1;
            break;

        case SDLK_x:
            if (!m_current.empty())
            {
                for (auto a : m_current)
                {
                    a->set_selected(false);
                }

                auto fn = [&](const auto &o)
                { return m_current.count(o.get()); };

                m_objects.erase(std::remove_if(m_objects.begin(), m_objects.end(), fn));
                m_current.clear();
            }
            break;

        case SDLK_z:
            for (auto a : m_current)
            {
                a->set_collision_enabled(!a->is_collision_enabled());
            }
            break;

        case SDLK_v:
            for (auto a : m_current)
            {
                a->set_active(!a->is_active());
            }
            break;

        case SDLK_1:
            m_objects.push_back(Navigator::create(m_renderer));
            m_objects.back()->set_position({m_mouse.x, m_mouse.y});
            break;

        case SDLK_2:
            m_walls.push_back(Wall::create(m_renderer, m_selection));
            m_selection.clear();
            break;

        case SDLK_ESCAPE:
            m_running = false;
            break;
        }
    }

    void on_mousebuttonup(const SDL_Event &event)
    {
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            bool found = false;

            for (const auto &o : m_objects)
            {
                if (o->is_active() && o->is_inside(m_mouse))
                {
                    found = true;

                    if (m_current.count(o.get()))
                    {
                        o->set_selected(false);
                        m_current.erase(o.get());
                    }
                    else
                    {
                        if ((SDL_GetModState() & KMOD_CTRL) == 0)
                        {
                            for (auto a : m_current)
                            {
                                a->set_selected(false);
                            }

                            m_current.clear();
                        }

                        o->set_selected(true);
                        m_current.insert(o.get());
                    }
                    break;
                }
            }

            if (!found)
            {
                for (auto a : m_current)
                {
                    a->set_selected(false);
                }

                m_current.clear();

                m_selection.push_back(m_mouse);
            }
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
            for (auto a : m_current)
            {
                a->set_selected(false);
            }

            m_current.clear();
            m_selection.clear();
        }
    }

    void poll_event()
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            EventGenerator::handle_event(event);
        }

        for (const auto &o : m_objects)
        {
            if (o->is_active())
            {
                o->tick();
            }
        }
    }

    void render()
    {
        SDL_SetRenderDrawColor(m_renderer, 50, 50, 50, 255);
        SDL_RenderClear(m_renderer);

        for (const auto &w : m_walls)
        {
            w->render(m_renderer);
        }

        for (const auto &l : m_objects)
        {
            l->render(m_renderer);

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

                // SDL_RenderSetViewport(m_renderer, &m_camera);

                if (collisions)
                {
                    SDL_SetRenderDrawColor(m_renderer, 0, 0, 255, 255);
                }
                else
                {
                    SDL_SetRenderDrawColor(m_renderer, 0, 255, 0, 255);
                }
            }
        }

        for (auto p : m_selection)
        {
            SDL_Rect rect;
            rect.w = 4;
            rect.h = 4;
            rect.x = p.x - 2;
            rect.y = p.y - 2;

            SDL_SetRenderDrawColor(m_renderer, 50, 125, 200, 255);
            SDL_RenderFillRect(m_renderer, &rect);
        }

        m_mouse_label->render(m_renderer);

        SDL_RenderPresent(m_renderer);
    }

    void run()
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
        }
    }

private:
    SDL_Window *m_window{nullptr};
    SDL_Renderer *m_renderer{nullptr};
    SDL_Rect m_camera;
    bool m_running{true};

    Point m_mouse;
    std::unique_ptr<Text> m_mouse_label;

    std::vector<std::unique_ptr<Wall>> m_walls;
    std::vector<std::unique_ptr<Navigator>> m_objects;

    std::vector<Point> m_selection;

    std::set<Navigator *> m_current;
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
    }

    return 0;
}