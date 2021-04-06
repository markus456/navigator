#pragma once

#include "common.hh"
#include "objects.hh"

#include <memory>

class Navigator : public Object
{
public:
    Navigator();

    static std::unique_ptr<Object> create();

    void tick() override;

private:
};
