/*
EGE - Extendable Game Engine
Copyright (c) Sppmacd 2020
*/

#pragma once

#include <ege/gfx/RenderStates.h>
#include <ege/gui/Widget.h>
#include <SFML/Graphics.hpp>

namespace EGE
{

class Label : public Widget
{
public:
    enum class Align
    {
        Left,
        Center,
        Right
    };

    explicit Label(Widget& parent)
    : Widget(parent) {}

    void setString(sf::String str);
    void setTextAlign(Align align);
    virtual void setPosition(EGE::Vec2d position) override;
    void setFontSize(int size);
    void setColor(sf::Color color) { m_color = color; setGeometryNeedUpdate(); }

protected:
    virtual void render(Renderer& renderer) const override;
    void setPositionInternal(EGE::Vec2d position);
    virtual void updateGeometry(Renderer& renderer) override;

    sf::String m_string;
    Align m_align = Align::Left;
    int m_fontSize = 15;
    sf::Color m_color = sf::Color::White;
    sf::Text m_text;
    EGE::Vec2d m_textPosition;
    std::shared_ptr<sf::Font> m_font;

private:
    virtual void setSize(EGE::Vec2d) {}
};

}
