#include <tuple>
#include <cstdint>

#include "world.hh"

using namespace std;

Navigator::Navigator()
    : Object({
          {-25.0, -25.0},
          {25.0, -25.0},
          {25.0, 25.0},
          {-25.0, 25.0},
      })
{
}

// static
std::unique_ptr<Object> Navigator::create()
{
    return std::make_unique<Navigator>();
}

void Navigator::tick()
{
}
