/*
EGE - Extendable Game Engine
Copyright (c) Sppmacd 2020
*/

#pragma once

#include <memory>
#include <string>
#include <vector>

namespace EGE
{

class ObjectMap;

class Object
{
public:
    virtual std::string toString() const = 0;

    virtual std::string asString() const { return ""; }
    virtual long long asInt() const { return 0; }
    virtual bool asBool() const { return false; }
    virtual std::shared_ptr<ObjectMap> asMap() const { return nullptr; }
    virtual double asFloat() const { return 0.0; }
    virtual std::vector<Object> asList() const { return {}; }

    virtual bool isString() const { return false; }
    virtual bool isInt() const { return false; }
    virtual bool isBool() const { return false; }
    virtual bool isMap() const { return false; }
    virtual bool isFloat() const { return false; }
    virtual bool isList() const { return false; }

    virtual std::shared_ptr<Object> copy() const = 0;
};

}
