/*
EGE - Extendable Game Engine
Copyright (c) Sppmacd 2020
*/

#include "GUIGameLoop.h"

namespace EGE
{

GUIGameLoop::GUIGameLoop()
{
}

GUIGameLoop::~GUIGameLoop()
{
    //m_currentGui is 'delete'd when removing eventhandler
    if(m_pendingGui)
        delete m_pendingGui;
}

EventResult GUIGameLoop::onLoad()
{
     m_profiler.start();
     m_profiler.startSection("load");
     if(m_resourceManager)
     {
         m_profiler.startSection("resourceManager");
         bool success = m_resourceManager->reload();
         success &= !m_resourceManager->isError();
         if(!success)
         {
            m_profiler.endSection();
            return EventResult::Failure;
         }
         m_profiler.endSection();
     }
     else
     {
         std::cerr << "000A EGE/gui: no ResourceManager set, setting to default GUIResourceManager" << std::endl;
         // TODO: implement GUIResourceManager
     }
     m_profiler.endSection();
     return EventResult::Success;
}

void GUIGameLoop::onTick(long long tickCount)
{
    m_profiler.startSection("tick");

    // Initialize pending GUI
    m_profiler.startSection("initPendingGUI");
    if(m_pendingGui)
    {
        DBG(GUI_DEBUG, "initPendingGUI");
        if(m_currentGui)
        {
            removeEventHandler(m_currentGui);
            // it's 'delete'd by eventhandler
        }
        m_currentGui = m_pendingGui;
        m_currentGui->onLoad();
        addEventHandler(SystemEvent::getTypeStatic(), std::shared_ptr<GUIScreen>(m_currentGui));
        m_pendingGui = nullptr;
    }

    // Call system event handlers
    m_profiler.endStartSection("systemEvents");
    if(m_systemWindow)
    {
        DBG(GUI_DEBUG, "systemEvents");
        m_systemWindow->callEvents(this, SystemWindow::WaitForEvents::No);
    }

    m_profiler.endStartSection("timers");
    updateTimers();

    m_profiler.endStartSection("logic");
    logicTick(tickCount);

    m_profiler.endStartSection("render");
    render();
    m_profiler.endSection();

    if(!m_systemWindow->isOpen())
        exit();

    // TODO: tick rate limit?

    m_profiler.endSection();
}

void GUIGameLoop::setCurrentGUIScreen(GUIScreen* screen, GUIScreenImmediateInit init)
{
    DBG(GUI_DEBUG, "setCurrentGUIScreen");

    if(init == GUIGameLoop::GUIScreenImmediateInit::Yes)
    {
        removeEventHandler(m_currentGui);
        // it's 'delete'd by eventhandler
        m_currentGui = screen;
        m_currentGui->onLoad();
        addEventHandler(SystemEvent::getTypeStatic(), std::shared_ptr<GUIScreen>(m_currentGui));
    }
    else
        m_pendingGui = screen;
}

std::weak_ptr<SFMLSystemWindow> GUIGameLoop::getWindow()
{
    return m_systemWindow;
}

void GUIGameLoop::setWindow(std::shared_ptr<SFMLSystemWindow> window)
{
    DBG(GUI_DEBUG, "setWindow");
    // TODO: automatically deletes previous window!
    // so it forces one GameLoop for window
    // so only one window is allowed currently
    m_systemWindow = std::shared_ptr<SFMLSystemWindow>(window);
}

std::weak_ptr<ResourceManager> GUIGameLoop::getResourceManager()
{
    return m_resourceManager;
}

void GUIGameLoop::setResourceManager(std::shared_ptr<ResourceManager> manager)
{
    m_resourceManager = manager;
}

void GUIGameLoop::render()
{
    if(m_systemWindow)
    {
        m_profiler.startSection("clear");
        m_systemWindow->clear();

        m_profiler.endStartSection("gui");
        if(m_currentGui)
            m_currentGui->render(*(m_systemWindow.get()));

        m_profiler.endStartSection("display");
        m_systemWindow->display();
        m_profiler.endSection();
    }
}

}
