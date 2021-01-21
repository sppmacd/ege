/*
*
*   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
*      ,----  ,----  ,----
*      |      |      |
*      |----  | --,  |----
*      |      |   |  |
*      '----  '---'  '----
*
*     Framework Library for Hexagon
*
*    Copyright (c) Sppmacd 2020 - 2021
*
*    Permission is hereby granted, free of charge, to any person obtaining a copy
*    of this software and associated documentation files (the "Software"), to deal
*    in the Software without restriction, including without limitation the rights
*    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*    copies of the Software, and to permit persons to whom the Software is
*    furnished to do so, subject to the following conditions:
*
*    The above copyright notice and this permission notice shall be included in all
*    copies or substantial portions of the Software.
*
*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*    SOFTWARE.
*
*   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
*/

#include "ObjectMap.h"

#include "PointerUtils.h"
#include "Serializable.h"

#include <ege/main/Config.h>

namespace EGE
{

ObjectMap::ObjectMap(const ObjectMap& map)
{
    for(auto pr: map)
    {
        if(pr.second)
            addObject(pr.first, pr.second->copy());
        else
            addObject(pr.first, nullptr);
    }
}

SharedPtr<Object> ObjectMap::copy() const
{
    return make<ObjectMap>(*this);
}

const SharedPtr<Object>& ObjectMap::addObject(std::string name, const SharedPtr<Object>& subObject)
{
    auto& ref = m_subObjects[name];
    ref = subObject;
    return ref;
}

const SharedPtr<Object>& ObjectMap::addObject(String name, const Serializable& subObject)
{
    return addObject(name, subObject.serialize());
}

const SharedPtr<Object>& ObjectMap::addFloat(std::string name, ObjectFloat::ValueType value)
{
    return addObject(name, make<ObjectFloat>(value));
}

const SharedPtr<Object>& ObjectMap::addInt(std::string name, ObjectInt::ValueType value, ObjectInt::Type type)
{
    return addObject(name, make<ObjectInt>(value, type));
}

const SharedPtr<Object>& ObjectMap::addUnsignedInt(std::string name, ObjectUnsignedInt::ValueType value, ObjectUnsignedInt::Type type)
{
    return addObject(name, make<ObjectUnsignedInt>(value, type));
}

const SharedPtr<Object>& ObjectMap::addList(std::string name, ObjectList::ValueType value)
{
    return addObject(name, make<ObjectList>(value));
}

const SharedPtr<Object>& ObjectMap::addString(std::string name, ObjectString::ValueType value)
{
    return addObject(name, make<ObjectString>(value));
}

ObjectMap::_Object ObjectMap::getObject(std::string name) const
{
    auto it = m_subObjects.find(name);
    if(it != m_subObjects.end())
        return it->second;
    return {};
}

bool ObjectMap::hasObject(std::string name) const
{
    auto it = m_subObjects.find(name);
    if(it != m_subObjects.end())
        return true;
    return false;
}

std::string ObjectMap::toString() const
{
    std::string str = "{";
    size_t counter = 0;
    for(auto pr: *this)
    {
        std::string key = pr.first;
        str += "\"" + key + "\":";

        if(pr.second)
            str += pr.second->toString();
        else
            str += "null";

        if(counter != m_subObjects.size() - 1)
        {
            str += ",";
        }
        counter++;
    }
    str += "}";
    return str;
}

ObjectMap::ValueType::const_iterator ObjectMap::begin() const
{
    return m_subObjects.begin();
}

ObjectMap::ValueType::const_iterator ObjectMap::end() const
{
    return m_subObjects.end();
}

size_t ObjectMap::size() const
{
    return m_subObjects.size();
}

std::map<std::string, SharedPtr<Object>> ObjectMap::asMap() const
{
    return m_subObjects;
}

SharedPtr<ObjectMap> ObjectMap::merge(SharedPtr<ObjectMap> other)
{
    auto me = Object::cast<ObjectMap>(copy()).value();

    if(other)
    {
        for(auto it: *other)
        {
            auto eo1 = me->getObject(it.first);
            auto existingObject = eo1.to<ObjectMap>();
            if(it.second->isMap() && existingObject.hasValue())
            {
                me->addObject(it.first, existingObject.value()->merge(Object::cast<ObjectMap>(it.second->copy()).value()));
            }
            else if(!existingObject.hasValue())
            {
                // Take only first object.
                me->addObject(it.first, it.second->copy());
            }
        }
    }

    return Object::cast<ObjectMap>(me).value();
}

}
