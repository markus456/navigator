#include "objects.hh"

#include <cassert>
#include <iostream>
#include <algorithm>

Object::Object(std::vector<Point> pts)
    : m_bounds(std::move(pts))
{
    Point center{0, 0};
    m_min = m_bounds.front();
    m_max = m_min;

    for (const auto &p : m_bounds)
    {
        assert(p.x >= 0 && p.y >= 0);

        m_min.x = std::min(m_min.x, p.x);
        m_min.y = std::min(m_min.y, p.y);
        m_max.x = std::max(m_max.x, p.x);
        m_max.y = std::max(m_max.y, p.y);
    }

    m_center.x = m_min.x + (m_max.x - m_min.x) / 2;
    m_center.y = m_min.y + (m_max.y - m_min.y) / 2;
}

// X and Y position of the object in the world, [0, 0] is the center of the world.
const Point &Object::position() const
{
    return m_pos;
}

// X and Y position of the object in the world, [0, 0] is the center of the world.
const Point &Object::center() const
{
    return m_center;
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
        p.rotate(rotation(), m_center);
        p += position();
    }

    return points;
}

std::vector<Line> Object::lines() const
{
    return to_lines(points());
}

std::vector<Line> Object::bounding_lines() const
{
    return to_lines(bounds());
}

std::pair<Point, Point> Object::bounding_rect() const
{
    return {m_min, m_max};
}

std::vector<Line> Object::to_lines(const std::vector<Point> &pts) const
{
    std::vector<Line> ln;

    for (size_t i = 0; i < pts.size() - 1; i++)
    {
        ln.emplace_back(pts[i], pts[i + 1]);
    }

    ln.emplace_back(pts[pts.size() - 1], pts[0]);

    return ln;
}

std::vector<Line> Object::scan_lines() const
{
    double y_min = std::numeric_limits<double>::max();
    double y_max = std::numeric_limits<double>::min();

    for (const auto &p : bounds())
    {
        y_min = std::min(y_min, p.y);
        y_max = std::max(y_max, p.y);
    }

    auto lines = bounding_lines();

    std::vector<Line> ln;

    for (int i = y_min; i < (int)y_max; i++)
    {
        Line l;
        l.first.x = -9e10;
        l.first.y = i;
        l.second.x = 9e10;
        l.second.y = i;

        auto [collided, pts] = get_collisions(lines, l);

        if (pts.size() == 1)
        {
            ln.emplace_back(pts.back(), pts.back());
        }
        else
        {
            while (!pts.empty())
            {
                if (pts.size() % 2 == 0)
                {
                    auto start = pts.back();
                    pts.pop_back();
                    auto end = pts.back();
                    pts.pop_back();
                    ln.emplace_back(start, end);
                }
                else
                {
                    ln.emplace_back(pts.back(), pts.back());
                    pts.pop_back();
                }
            }
        }
    }

    return ln;
}

// Implements this
// https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect/565282#565282
std::pair<bool, std::vector<Point>> Object::get_collisions(const std::vector<Line> &my_lines, const Line &line)
{
    const Point &p1 = line.first;
    const Point &p2 = line.second;
    std::vector<Point> points;

    auto check_intersection = [&](auto q1, auto q2)
    {
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

    for (auto l : my_lines)
    {
        check_intersection(l.first, l.second);
    }

    return {!points.empty(), points};
}

// Implements this
// https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect/565282#565282
std::pair<bool, std::vector<Point>> Object::get_collisions(const Line &line) const
{
    return get_collisions(lines(), line);
}

std::pair<bool, std::vector<Point>> Object::get_collisions(const Object &other) const
{
    bool rval = false;
    std::vector<Point> points;

    for (const auto &line : lines())
    {
        auto [collided, pts] = other.get_collisions(line);

        if (collided)
        {
            rval = true;
            points.insert(points.end(), pts.begin(), pts.end());
        }
    }

    return {rval, points};
}
