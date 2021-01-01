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

#include "CameraObject2D.h"

#include <SFML/Graphics.hpp>

namespace EGE
{

void CameraObject2D::render(Renderer& renderer) const
{
    if constexpr(CAMERA_DEBUG)
    {
        sf::VertexArray varr(sf::LineStrip, 5);
        sf::Vector2f pos(getPosition().x, getPosition().y);
        varr.append(sf::Vertex(sf::Vector2f(-30.f, -30.f) + pos, sf::Color::Yellow));
        varr.append(sf::Vertex(sf::Vector2f(30.f, 30.f) + pos, sf::Color::Yellow));
        varr.append(sf::Vertex(sf::Vector2f(-30.f, 30.f) + pos, sf::Color::Yellow));
        varr.append(sf::Vertex(sf::Vector2f(30.f, -30.f) + pos, sf::Color::Yellow));
        varr.append(sf::Vertex(sf::Vector2f(-30.f, -30.f) + pos, sf::Color::Yellow));
        renderer.getTarget().draw(varr);
    }
}

}
