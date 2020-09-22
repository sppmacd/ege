/*
EGE - Extendable Game Engine
Copyright (c) Sppmacd 2020
*/

#include "GUIScreen.h"

#include "GUIGameLoop.h"

namespace EGE
{

void CompoundWidget::onResize(sf::Event::SizeEvent& event)
{
    for(auto widget: m_childWidgets)
    {
        ASSERT(widget);
        widget->onResize(event);
    }
}

void CompoundWidget::onTextEnter(sf::Event::TextEvent& event)
{
    for(auto widget: m_childWidgets)
    {
        ASSERT(widget);
        widget->onTextEnter(event);
    }
}

void CompoundWidget::onKeyPress(sf::Event::KeyEvent& event)
{
    for(auto widget: m_childWidgets)
    {
        ASSERT(widget);
        widget->onKeyPress(event);
    }
}

void CompoundWidget::onKeyRelease(sf::Event::KeyEvent& event)
{
    for(auto widget: m_childWidgets)
    {
        ASSERT(widget);
        widget->onKeyRelease(event);
    }
}

void CompoundWidget::onMouseWheelScroll(sf::Event::MouseWheelScrollEvent& event)
{
    for(auto widget: m_childWidgets)
    {
        ASSERT(widget);
        widget->onMouseWheelScroll(event);
    }
}

void CompoundWidget::onMouseButtonPress(sf::Event::MouseButtonEvent& event)
{
    sf::Vector2f position(event.x, event.y);
    for(auto widget: m_childWidgets)
    {
        ASSERT(widget);

        // Change focused widget.
        if(widget->isMouseOver(position) && event.button == sf::Mouse::Left)
        {
            if(m_focusedWidget)
                m_focusedWidget->onLossFocus();

            m_focusedWidget = widget;
            m_focusedWidget->onGainFocus();

            widget->onMouseButtonPress(event);
        }
    }
}

void CompoundWidget::onMouseButtonRelease(sf::Event::MouseButtonEvent& event)
{
    for(auto widget: m_childWidgets)
    {
        ASSERT(widget);
        widget->onMouseButtonRelease(event);
    }
}

void CompoundWidget::onMouseMove(sf::Event::MouseMoveEvent& event)
{
    for(auto widget: m_childWidgets)
    {
        ASSERT(widget);
        widget->onMouseMove(event);
    }
}

void CompoundWidget::onJoystickButtonPress(sf::Event::JoystickButtonEvent& event)
{
    for(auto widget: m_childWidgets)
    {
        widget->onJoystickButtonPress(event);
    }
}

void CompoundWidget::onJoystickButtonRelease(sf::Event::JoystickButtonEvent& event)
{
    for(auto widget: m_childWidgets)
    {
        widget->onJoystickButtonRelease(event);
    }
}

void CompoundWidget::onJoystickMove(sf::Event::JoystickMoveEvent& event)
{
    for(auto widget: m_childWidgets)
    {
        widget->onJoystickMove(event);
    }
}

void CompoundWidget::onJoystickConnect(sf::Event::JoystickConnectEvent& event)
{
    for(auto widget: m_childWidgets)
    {
        widget->onJoystickConnect(event);
    }
}

void CompoundWidget::onJoystickDisconnect(sf::Event::JoystickConnectEvent& event)
{
    for(auto widget: m_childWidgets)
    {
        widget->onJoystickDisconnect(event);
    }
}

void CompoundWidget::onTouchBegin(sf::Event::TouchEvent& event)
{
    for(auto widget: m_childWidgets)
    {
        widget->onTouchBegin(event);
    }
}

void CompoundWidget::onTouchMove(sf::Event::TouchEvent& event)
{
    for(auto widget: m_childWidgets)
    {
        widget->onTouchMove(event);
    }
}

void CompoundWidget::onTouchEnd(sf::Event::TouchEvent& event)
{
    for(auto widget: m_childWidgets)
    {
        widget->onTouchEnd(event);
    }
}

void CompoundWidget::onSensorChange(sf::Event::SensorEvent& event)
{
    for(auto widget: m_childWidgets)
    {
        widget->onSensorChange(event);
    }
}

void CompoundWidget::onUpdate(long long tickCounter)
{
    Widget::onUpdate(tickCounter);
    for(auto widget: m_childWidgets)
    {
        widget->onUpdate(tickCounter);
    }
}

void CompoundWidget::render(sf::RenderTarget& target)
{
    // TODO: draw only visible widgets
    for(auto widget: m_childWidgets)
    {
        Widget::render(target);
        widget->render(target);
    }
}

void CompoundWidget::addWidget(std::shared_ptr<Widget> widget)
{
    DUMP(GUI_DEBUG, "addWidget");
    // deferredInvoke to allow adding and removing widgets inside event handlers
    getLoop()->deferredInvoke([this,widget]() {
        DUMP(GUI_DEBUG, widget.get());
        ASSERT(widget.get());
        widget->onLoad();

        // allow widgets know about window's size when creating
        sf::Vector2u wndSize = getLoop()->getWindow().lock()->getSize();
        sf::Event::SizeEvent event{wndSize.x, wndSize.y};
        widget->onResize(event);

        m_childWidgets.push_back(widget);
    });
}

void CompoundWidget::removeWidget(Widget* widget)
{
    // deferredInvoke to allow adding and removing widgets inside event handlers
    getLoop()->deferredInvoke([this,widget]() {
        for(auto it = m_childWidgets.begin(); it != m_childWidgets.end(); it++)
        {
            if(it->get() == widget)
            {
                if(widget == m_focusedWidget.get())
                    m_focusedWidget = nullptr;

                m_childWidgets.erase(it);
                return;
            }
        }
    });
}

}