#include "objects.hh"

#include <cassert>
#include <iostream>

Object::Object(std::vector<Point> pts)
    : m_bounds(std::move(pts))
{
    Point center{0, 0};

    for (const auto &p : m_bounds)
    {
        m_rad = std::max(center.distance(p), m_rad);
    }
}

// X and Y position of the object in the world, [0, 0] is the center of the world.
const Point &Object::position() const
{
    return m_pos;
}

void Object::set_position(Point p)
{
    m_pos = p;
}

const std::vector<Point> &Object::bounds() const
{
    return m_bounds;
}

// Rotation in degrees from north, clockwise.
double Object::rotation() const
{
    return m_dir;
}

void Object::set_rotation(double d)
{
    m_dir = d;
}

std::vector<Point> Object::points() const
{
    auto points = bounds();

    for (auto &p : points)
    {
        p.rotate(rotation());
        p += position();
    }

    return points;
}

std::vector<std::pair<Point, Point>> Object::lines() const
{
    std::vector<std::pair<Point, Point>> ln;
    auto pts = points();

    for (size_t i = 0; i < pts.size() - 1; i++)
    {
        ln.emplace_back(pts[i], pts[i + 1]);
    }

    ln.emplace_back(pts[pts.size() - 1], pts[0]);

    return ln;
}

double Object::radius() const
{
    return m_rad;
}

// Implements this
// https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect/565282#565282
std::pair<bool, std::vector<Point>> Object::get_collisions(const Line &line) const
{
    const Point &p1 = line.first;
    const Point &p2 = line.second;
    std::vector<Point> points;

    auto check_intersection = [&](auto q1, auto q2) {
        auto r = p2 - p1;
        auto s = q2 - q1;
        auto rxs = r.cross(s);
        auto qpxr = (q1 - p1).cross(r);
        auto t = (q1 - p1).cross(s) / r.cross(s);
        auto u = (q1 - p1).cross(r) / r.cross(s);

        if (rxs == 0)
        {
            if (qpxr == 0)
            {
                auto t0 = (q1 - p1).dot(r) / r.dot(r);
                auto t1 = t0 + s.dot(r) / r.dot(r);

                if ((t0 > 0 && t0 < 1) || (t1 > 0 && t1 < 1))
                {
                    points.push_back({p1 + r * t});
                }
            }
            else
            {
                return;
            }
        }

        if (u >= 0.0 && u <= 1.0 && t >= 0.0 && t <= 1.0)
        {
            points.push_back({p1 + r * t});
        }
    };

    for (auto l : lines())
    {
        check_intersection(l.first, l.second);
    }

    return {!points.empty(), points};
}

std::pair<bool, std::vector<Point>> Object::get_collisions(const Object &other) const
{
    bool rval = false;
    std::vector<Point> points;

    for (const auto &line : lines())
    {
        if (position().distance(other.position()) < radius() + other.radius() + 1.0)
        {
            auto [collided, pts] = other.get_collisions(line);

            if (collided)
            {
                rval = true;
                points.insert(points.end(), pts.begin(), pts.end());
            }
        }
    }

    return {rval, points};
}
