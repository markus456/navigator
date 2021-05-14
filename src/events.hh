#include "common.hh"

class EventListener
{
    virtual ~EventListener() = default;

    virtual std::vector<int> get_events() = 0;

    virtual void handle_event(const SDL_Event &event) = 0;
};