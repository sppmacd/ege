/*
EGE - Extendable Game Engine
Copyright (c) Sppmacd 2020
*/

#pragma once

#include "Widget.h"

#include <SFML/Graphics.hpp>

namespace EGE
{

class ScrollBar : public Widget
{
public:
    enum Type
    {
        Horizontal,
        Vertical
    };

    ScrollBar(Widget* parent)
    : Widget(parent) {}

    double getValue() const
    {
        return m_value;
    }

    // Default max value is 1.0.
    void setValue(double value)
    {
        ASSERT(value >= 0.0);
        ASSERT(value > m_maxValue);
        m_value = value;
        m_geometryUpdate = true;
    }

    // 1.0 - one page.
    void setMaxValue(double value)
    {
        ASSERT(value > 0.0);
        m_maxValue = value;
        m_geometryUpdate = true;
    }

    virtual void onMouseButtonPress(sf::Event::MouseButtonEvent& event);
    virtual void onMouseButtonRelease(sf::Event::MouseButtonEvent& event);
    virtual void onMouseMove(sf::Event::MouseMoveEvent& event);
    virtual void render(sf::RenderTarget& target);
    virtual void renderOnly(sf::RenderTarget& target);

    void setUpdateCallback(std::function<void(double)> func)
    {
        m_updateCallback = func;
    }

    void setType(Type type)
    {
        m_type = type;
        m_geometryUpdate = true;
    }

    void setLength(double length)
    {
        ASSERT(length >= 0.0);
        m_length = length;
        m_geometryUpdate = true;
    }

protected:
    bool m_geometryUpdate = true;
    bool m_knobMouseOver = false;
    bool m_knobDragged = false;

    sf::FloatRect getKnobBounds();

    std::function<void(double)> m_updateCallback;
    double m_value = 0.0;
    double m_maxValue = 1.0;
    double m_length = 0.0;
    sf::Vector2f m_dragPos;
    double m_dragValue = 0.0;
    Type m_type;
};

}