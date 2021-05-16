#include "../objects.hh"

#include <vector>
#include <iostream>

struct TestObject : public Object
{
public:
    TestObject()
        : Object({
              {0, 0},
              {0, 10},
              {10, 10},
              {10, 0},
          })
    {
    }

    void tick()
    {
    }

    void state_changed(Object::ChangeType type)
    {
    }
};

int main(int argc, char **argv)
{
    TestObject n1, n2;

    n1.set_position({0, 0});
    n2.set_position({5, 5});
    std::cout << "Collision 1: " << (n1.collision(n2) ? "Yes" : "No") << std::endl;

    n2.set_position({0, 2.05});
    n2.set_rotation(45);
    std::cout << "Collision 2: " << (n1.collision(n2) ? "Yes" : "No") << std::endl;

    n2.set_position({0, 20});
    n2.set_rotation(0);
    std::cout << "Collision 3: " << (n1.collision(n2) ? "Yes" : "No") << std::endl;

    std::string line;
    std::cin >> line;
    return 0;
}
