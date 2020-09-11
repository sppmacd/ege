/*
EGE - Extendable Game Engine
Copyright (c) Sppmacd 2020
*/

#pragma once

#include <ege/main/Config.h>
#include <ege/util/ObjectMap.h>
#include <memory>
#include <string>

namespace EGE
{

class ControlObject
{
public:
    ControlObject(std::string type, std::shared_ptr<ObjectMap> args = make<ObjectMap>())
    : m_type(type)
    , m_args(args) {}

    std::string getType() const
    {
        return m_type;
    }
    std::shared_ptr<ObjectMap> getArgs() const
    {
        return std::static_pointer_cast<ObjectMap>(m_args->copy());
    }

private:
    std::string m_type;
    std::shared_ptr<ObjectMap> m_args;
};

}
