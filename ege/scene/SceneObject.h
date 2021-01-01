/*
EGE - Extendable Game Engine
Copyright (c) Sppmacd 2020
*/

#pragma once

#include "ObjectRenderer.h"

#include <SFML/Graphics.hpp>
#include <ege/controller/Controllable.h>
#include <ege/debug/Logger.h>
#include <ege/gfx/RenderStates.h>
#include <ege/gfx/Renderable.h>
#include <ege/gpo/GameplayObject.h>
#include <ege/gui/Animatable.h>
#include <ege/util/Serializable.h>

namespace EGE
{

class Scene;

class SceneObject : public Animatable, public GameplayObject, public Controllable, public Renderable, public sf::NonCopyable
{
public:
    SceneObject(EGE::Scene& owner, std::string typeId)
    : GameplayObject(typeId), m_owner(owner)
    {
        log(LogLevel::Debug) << "SceneObject::SceneObject(" << typeId << ") " << this;
    }

    virtual ~SceneObject();

    virtual void onUpdate(long long tickCounter);

    virtual void render(Renderer& renderer) const override
    {
        if(m_renderer)
            m_renderer->render(renderer);
    }
    virtual void updateGeometry(Renderer& renderer) override
    {
        if(m_renderer)
            m_renderer->updateGeometry(renderer);
    }

    bool isDead() const { return m_dead; }

    IdType getObjectId() const { return m_id; }
    void setObjectId(IdType id) { if(!m_id) m_id = id; }

    std::string getName() const { return m_name; }
    void setName(std::string name) { m_name = name; setChanged(); }

    virtual std::shared_ptr<ObjectMap> serialize() const;
    virtual bool deserialize(std::shared_ptr<ObjectMap>);

    virtual std::shared_ptr<ObjectMap> serializeMain() const;
    virtual bool deserializeMain(std::shared_ptr<ObjectMap>);

    virtual std::shared_ptr<ObjectMap> serializeExtended() const;
    virtual bool deserializeExtended(std::shared_ptr<ObjectMap>);

    // Clears main and extended changed flags. Called by server.
    // FIXME: it should be only callable by Server.

    void clearMainChangedFlag() { m_mainChanged = false; }
    void clearExtendedChangedFlag() { m_extendedChanged = false; }

    bool getMainChangedFlag() const { return m_mainChanged; }
    bool getExtendedChangedFlag() const { return m_extendedChanged; }
    bool didChangeSinceLoad() const { return m_changedSinceLoad; }

    void setDead() { m_dead = true; }
    Scene& getOwner() const { return m_owner; }
    void setRenderer(std::shared_ptr<ObjectRenderer> renderer) { m_renderer = std::static_pointer_cast<ObjectRenderer>(renderer); }

    void setParent(SceneObject* object);

protected:
    void setMainChanged() { m_mainChanged = true; setChanged(); }
    void setExtendedChanged() { m_extendedChanged = true; setChanged(); }
    void setChanged() { m_changedSinceLoad = true; }

    Scene& m_owner;
    bool m_dead = false;
    IdType m_id = 0;
    std::string m_name;
    bool m_mainChanged = true;
    bool m_extendedChanged = true;
    bool m_changedSinceLoad = false;
    std::shared_ptr<ObjectRenderer> m_renderer;

    Set<SceneObject*> m_children;
    SceneObject* m_parent = nullptr;

    friend class ObjectRenderer;
    friend class Scene;
};

}
