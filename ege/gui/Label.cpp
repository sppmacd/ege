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

#include "Label.h"
#include "GUIGameLoop.h"

namespace EGE
{

void Label::setString(sf::String str)
{
    m_string = str;
    setGeometryNeedUpdate();
}

void Label::setTextAlign(Align align)
{
    m_align = align;
    setGeometryNeedUpdate();
}

void Label::setFontSize(int size)
{
    m_fontSize = size;
    setGeometryNeedUpdate();
}

void Label::updateGeometry(Renderer& renderer)
{
    // FIXME: label is weirdly clipped (':('; '???')
    ASSERT(getLoop().getResourceManager());
    if(!m_font)
    {
        m_font = getLoop().getResourceManager()->getDefaultFont();
    }
    sf::Text text(m_string, *m_font, m_fontSize);
    sf::FloatRect bounds = text.getLocalBounds();
    bounds.height += 5.f * m_fontSize / 20.f; //SFML text bounds bug??
    bounds.width += 1.f * m_fontSize / 15.f;

    if(getRawSize().x.unit() == EGE_LAYOUT_AUTO || getRawSize().y.unit() == EGE_LAYOUT_AUTO)
    {
        // Set to text bounds.
        setSize({bounds.width, bounds.height});
    }

    // Layouting
    runLayoutUpdate();

    Vec2d position;

    switch(m_align)
    {
        case Align::Left:
            text.setOrigin(0.f, 0.f);
            position = Vec2d();
            break;
        case Align::Center:
            text.setOrigin(bounds.width / 2.0, bounds.height / 2.0);
            position = getSize() / 2.0;
            break;
        case Align::Right:
            text.setOrigin(bounds.width, bounds.height);
            position = getSize();
            break;
    }

    text.setPosition(position.x, position.y);
    text.setFillColor(m_color);
    m_text = text;
}

void Label::render(Renderer& renderer) const
{
    renderer.getTarget().draw(m_text);

    Widget::render(renderer);
}

}
