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

#include "SceneLoader.h"

#include <ege/debug/Dump.h>
#include <ege/debug/Logger.h>
#include <ege/scene/Scene.h>

#include <fstream>

namespace EGE
{

bool SceneLoader::loadRegistry(SceneObjectRegistry& registry, String fileName, const IOStreamConverter& converter)
{
    SharedPtr<Object> data;
    std::ifstream file(CommonPaths::resourceDir() + "/" + fileName);
    if(!file.good())
    {
        err() << "Failed to open registry file";
        return false; // couldn't open file
    }

    if(!(file >> objectIn(data, converter)))
    {
        err() << "Invalid registry JSON";
        return false; // invalid JSON
    }

    auto data_map = Object::cast<ObjectMap>(data);
    if(!data_map.hasValue())
    {
        err() << "Data must be a map";
        return false; // it's not a map!
    }

    for(auto& pr: *data_map.value())
    {
        auto sodata = Object::cast<ObjectMap>(pr.second);
        if(!sodata.hasValue())
        {
            err() << "Data entry must be a map";
            return false; // entry not a map
        }

        auto sd_baseClass = sodata.value()->getObject("baseClass").asString().valueOr("SceneObject2D");

        SharedPtr<SceneObjectType> sotype;
        if(sd_baseClass == "SceneObject2D")
            sotype = make<SceneObjectType2D>(pr.first);
        else
        {
            err() << "Invalid base class for SceneObjectType";
            return false;
        }

        auto sd_data = sodata.value()->getObject("data").to<ObjectMap>();
        if(!sd_data.hasValue())
        {
            err() << "Registry entry must be a map";
            return false; // data is not a map
        }

        log() << "Loading data for SceneObjectType: " << pr.first;
        if(!sotype->deserialize(sd_data.value()))
        {
            err() << "Invalid SceneObjectType";
            return false; // invalid sceneobjecttype!
        }
        registry.addType(sotype);
    }
    return true;
}

SharedPtr<ObjectMap> SceneLoader::serializeSceneObjects() const
{
    auto data = make<ObjectMap>();

    // Add dynamic objects
    auto objects = make<ObjectList>();
    for(auto& sObj : m_scene.m_objects)
    {
        auto entry = sObj.second->serialize();
        entry->addString("typeId", sObj.second->getType().getId());
        objects->addObject(entry);
    }
    log(LogLevel::Debug) << "SceneLoader finished saving " << objects->size() << " objects";
    data->addObject("objects", objects);

    // Add static objects (if changed)
    auto staticObjects = make<ObjectList>();
    for(auto& sObj : m_scene.m_staticObjects)
    {
        if(sObj.second->didChangeSinceLoad())
        {
            auto entry = sObj.second->serialize();
            entry->addString("typeId", sObj.second->getType().getId());
            staticObjects->addObject(entry);
        }
    }
    log(LogLevel::Debug) << "SceneLoader finished saving " << staticObjects->size() << " static objects";
    data->addObject("staticObjects", staticObjects);

    return data;
}

SharedPtr<SceneObject> SceneLoader::loadObject(Optional<SharedPtr<ObjectMap>> objMap)
{
    if(!objMap.hasValue())
    {
        err() << "SceneObject description is not a Map!";
        return nullptr;
    }

    auto typeId = objMap.value()->getObject("typeId").asString();
    if(!typeId.hasValue())
    {
        err() << "SceneObject has no valid type ID!";
        return nullptr;
    }

    return m_scene.createObject(typeId.value(), objMap.value());
}

bool SceneLoader::deserializeSceneObjects(SharedPtr<ObjectMap> data)
{
    // Load all static objects that changed from installation scene
    auto staticObjects = data->getObject("staticObjects").to<ObjectList>();
    if(!staticObjects.hasValue())
        return false;

    for(auto obj: *staticObjects.value())
    {
        auto objMap = Object::cast<ObjectMap>(obj);
        auto sceneObject = loadObject(objMap);

        if(!sceneObject)
            continue;

        m_scene.addStaticObject(sceneObject, true);
    }

    // Load all "dynamic" objects
    auto objects = data->getObject("objects").to<ObjectList>();
    if(!objects.hasValue())
        return false;

    for(auto obj: *objects.value())
    {
        auto objMap = Object::cast<ObjectMap>(obj);
        auto sceneObject = loadObject(objMap);

        if(!sceneObject)
            continue;

        m_scene.addObject(sceneObject);
    }

    log(LogLevel::Debug) << "SceneLoader finished loading with " << m_scene.m_objects.size() << " objects";

    return true;
}

bool SceneLoader::deserializeStaticSceneObjects(SharedPtr<ObjectMap> data)
{
    // Load all objects and add them as static objects.
    auto objects = data->getObject("objects").to<ObjectList>();
    if(!objects.hasValue())
        return false;

    for(auto obj: *objects.value())
    {
        auto objMap = Object::cast<ObjectMap>(obj);
        auto sceneObject = loadObject(objMap);

        if(!sceneObject)
            continue;

        m_scene.addStaticObject(sceneObject);
    }

    log(LogLevel::Debug) << "SceneLoader finished loading with " << m_scene.m_staticObjects.size() << " static objects";

    return true;
}

bool SceneLoader::saveScene(String fileName, const IOStreamConverter& converter) const
{
    log() << "Saving scene to " << fileName;
    std::ofstream file(CommonPaths::saveDir() + "/" + fileName);
    if(!file.good())
    {
        err() << "Scene saving failed - failed to open file!";
        return false;
    }

    auto object = serializeSceneObjects();
    if(!object)
    {
        err() << "Scene loading failed - failed to generate data!";
        return false;
    }

    return converter.out(file, *object);
}

bool SceneLoader::loadScene(String fileName, const IOStreamConverter& converter)
{
    log() << "Loading scene from " << fileName;
    std::ifstream file(CommonPaths::saveDir() + "/" + fileName);
    if(!file.good())
    {
        err() << "Scene loading failed - failed to open file!";
        return false;
    }

    SharedPtr<Object> object;
    if(!converter.in(file, object))
    {
        err() << "Scene loading failed - failed to parse file data!";
        return false;
    }

    auto objectMap = object->cast<ObjectMap>(object);
    if(!objectMap.hasValue())
    {
        err() << "Scene loading failed - invalid data format!";
        return false;
    }

    if(!deserializeSceneObjects(objectMap.value()))
    {
        err() << "Scene loading failed - failed to load data!";
        return false;
    }
    return true;
}

bool SceneLoader::loadStaticObjects(String fileName, const IOStreamConverter& converter)
{
    log() << "Loading static objects from " << fileName;
    std::ifstream file(CommonPaths::resourceDir() + "/" + fileName);
    if(!file.good())
    {
        err() << "Static object loading failed - failed to open file!";
        return false;
    }

    SharedPtr<Object> object;
    if(!converter.in(file, object))
    {
        err() << "Static object loading failed - failed to parse file data!";
        return false;
    }

    auto objectMap = object->cast<ObjectMap>(object);
    if(!objectMap.hasValue())
    {
        err() << "Static object loading failed - invalid data format!";
        return false;
    }

    if(!deserializeStaticSceneObjects(objectMap.value()))
    {
        err() << "Static object loading failed - failed to load data!";
        return false;
    }
    return true;
}

bool SceneLoader::loadSceneAndSave(String saveName, String sceneName, const IOStreamConverter& converter)
{
    if(!loadStaticObjects(sceneName, converter))
    {
        log(LogLevel::Critical) << "Failed to load predefined scene!";
        return false;
    }

    if(!loadScene(saveName, converter))
        // It's nothing wrong, we will just create a new save!
        log(LogLevel::Warning) << "Empty or invalid save: " << saveName;

    return true;
}

}
