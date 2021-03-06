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

#include "PartStub.h"

#include "CirclePart.h"
#include "PolygonPart.h"
#include "RectanglePart.h"
#include "TexturedPart.h"

#include "../SceneObject.h"

#include <ege/debug/Logger.h>

namespace EGE
{

PartCreatorMap::PartCreatorMap()
{
    // Add default parts
    add("Circle", EGE_PART_CREATOR(EGE::CirclePart));
    add("Polygon", EGE_PART_CREATOR(EGE::PolygonPart));
    add("Rectangle", EGE_PART_CREATOR(EGE::RectanglePart));
    add("Textured", EGE_PART_CREATOR(EGE::TexturedPart));
}

PartCreatorMap PartStub::PartCreators;

SharedPtr<Part> PartStub::makeInstance(SceneObject& sobject)
{
    ege_log.debug() << "Creating instance of part for SO " << sobject.getName();
    auto partCreator = PartStub::PartCreators.get(m_type);
    if(!partCreator)
    {
        ege_err.error() << "No such part with type: " << m_type;
        return nullptr;
    }

    auto part = (*partCreator)(sobject);
    if(!part)
    {
        ege_err.error() << "Failed to create part!";
        return nullptr;
    }

    if(!part->deserialize(m_map))
        return nullptr;
    return part;
}

SharedPtr<ObjectMap> PartStub::serialize() const
{
    return m_map;
}

bool PartStub::deserialize(SharedPtr<ObjectMap> data)
{
    m_map = Object::cast<ObjectMap>(data->copy()).value();
    m_type = data->getObject("type").asString().valueOr("");
    if(m_type.empty())
        return false;
    return true;
}

}

