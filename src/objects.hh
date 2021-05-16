#pragma once

#include <tuple>
#include <cstdint>
#include <vector>
#include <cmath>
#include <functional>

struct Point
{
    double x;
    double y;

    inline void operator+=(const Point &rhs)
    {
        x += rhs.x;
        y += rhs.y;
    }

    inline void operator-=(const Point &rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
    }

    inline void operator*=(double val)
    {
        x *= val;
        y *= val;
    }

    void rotate(double d, Point center)
    {
        const double PI = 3.1415;
        double r = d * PI * 2 / 360;
        Point tmp = *this;
        tmp -= center;
        double xi = tmp.x * cos(r) - tmp.y * sin(r);
        double yi = tmp.x * sin(r) + tmp.y * cos(r);
        tmp.x = xi;
        tmp.y = yi;
        tmp += center;
        *this = tmp;
    }

    void rotate(double d)
    {
        const double PI = 3.1415;
        double r = d * PI * 2 / 360;
        double xi = x * cos(r) - y * sin(r);
        double yi = x * sin(r) + y * cos(r);
        x = xi;
        y = yi;
    }

    double cross(const Point &rhs) const
    {
        return x * rhs.y - y * rhs.x;
    }

    double dot(const Point &rhs) const
    {
        return x * rhs.x + y * rhs.y;
    }

    double distance(const Point &rhs) const
    {
        return sqrt(pow(rhs.x - x, 2) + pow(rhs.y - y, 2));
    }
};

inline Point operator+(const Point &lhs, const Point &rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

inline Point operator-(const Point &lhs, const Point &rhs)
{
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

inline Point operator*(const Point &lhs, double val)
{
    return {lhs.x * val, lhs.y * val};
}

inline bool operator==(const Point &lhs, const Point &rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

using Line = std::pair<Point, Point>;

// An object that has a position, rotation and a polygon that defines the bounds.
struct Object
{
    // Construct an object with bounds consisting of a polygon.
    Object(std::vector<Point> lines);

    virtual ~Object() = default;

    // Called when the world advances one tick
    virtual void tick() = 0;

    // X and Y position of the object in the world, [0, 0] is the center of the world.
    const Point &position() const;

    // Set position
    void set_position(Point p);

    // Rotation in degrees from north, clockwise.
    double rotation() const;

    // Set rotation
    void set_rotation(double d);

    // Bounds of the object, not translated or rotated
    const std::vector<Point> &bounds() const;

    // X and Y position of the object center. Rotations are done around this point.
    const Point &center() const;

    // Get the set of lines that form the polygon
    std::vector<Line> bounding_lines() const;

    // Get the set of horizontal lines that form the polygon
    std::vector<Line> scan_lines() const;

    // The bounding rectangle
    std::pair<Point, Point> bounding_rect() const;

    // Get the bounding polygon as points, rotated and translated to world coordinates
    std::vector<Point> points() const;

    // Get the set of lines that form the polygon, in world coordinates
    std::vector<Line> lines() const;

    // Get points where the lines collide with the given line
    static std::pair<bool, std::vector<Point>> get_collisions(const std::vector<Line> &my_lines, const Line &line);

    // Get the points where the two objects collide
    std::pair<bool, std::vector<Point>> get_collisions(const Object &other) const;

    // Get points where this object collides with the given line
    std::pair<bool, std::vector<Point>> get_collisions(const Line &line) const;

    // Check if this object collides with another object
    bool collision(const Object &other) const
    {
        return get_collisions(other).first;
    }

    // Check if the line intersects this object
    bool collision(const Line &line) const
    {
        return get_collisions(line).first;
    }

    // Check if the point is inside this object
    bool is_inside(const Point &p) const
    {
        Point p1{p.x, 9e10};
        Point p2{p.x, -9e10};

        auto collisions1 = get_collisions({p, p1}).second;
        auto collisions2 = get_collisions({p, p2}).second;

        return collisions1.size() % 2 && collisions2.size() % 2;
    }

private:
    std::vector<Line> to_lines(const std::vector<Point> &pts) const;

    Point m_pos{0, 0};
    std::vector<Point> m_bounds;
    double m_dir{0.0};
    Point m_min{0.0};
    Point m_max{0.0};
    Point m_center{0.0};
};
