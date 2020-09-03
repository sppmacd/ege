/*
EGE - Extendable Game Engine
Copyright (c) Sppmacd 2020
*/

#include "EGEPacket.h"

#include <ege/util/ObjectInt.h>

namespace EGE
{

std::shared_ptr<EGEPacket> EGEPacket::generate_Data(std::shared_ptr<ObjectMap> data)
{
    ASSERT(data);
    EGEPacket::appendUID(data);
    return std::make_shared<EGEPacket>(EGEPacket::Type::_Data, data);
}

std::shared_ptr<EGEPacket> EGEPacket::generate_Ping()
{
    return std::make_shared<EGEPacket>(EGEPacket::Type::_Ping, nullptr);
}

std::shared_ptr<EGEPacket> EGEPacket::generate_Pong()
{
    return std::make_shared<EGEPacket>(EGEPacket::Type::_Pong, nullptr);
}

std::shared_ptr<EGEPacket> EGEPacket::generate_ProtocolVersion(int value)
{
    std::shared_ptr<ObjectMap> object = std::make_shared<ObjectMap>();
    object->addObject("value", std::make_shared<ObjectInt>(value));
    return std::make_shared<EGEPacket>(EGEPacket::Type::_ProtocolVersion, object);
}

std::shared_ptr<EGEPacket> EGEPacket::generateSResult(long long uid, std::shared_ptr<ObjectMap> userData)
{
    userData->addObject("uid", std::make_shared<ObjectInt>(uid));
    return std::make_shared<EGEPacket>(EGEPacket::Type::SResult, userData);
}

std::shared_ptr<EGEPacket> EGEPacket::generateCLogin(std::shared_ptr<ObjectMap> userData) //SResult
{
    if(userData)
        EGEPacket::appendUID(userData);
    else
    {
        userData = std::make_shared<ObjectMap>();
        userData->addObject("uid", std::make_shared<ObjectInt>(generateUID()));
    }
    return std::make_shared<EGEPacket>(EGEPacket::Type::CLogin, userData);
}

std::shared_ptr<EGEPacket> EGEPacket::generateSLoginRequest(std::shared_ptr<ObjectMap> userData)
{
    return std::make_shared<EGEPacket>(EGEPacket::Type::SLoginRequest, userData);
}

std::shared_ptr<EGEPacket> EGEPacket::generateSDisconnectReason(std::string message)
{
    std::cerr << "reason: " << message << std::endl;
    return nullptr;
}

std::shared_ptr<EGEPacket> EGEPacket::generateSSceneObjectCreation(std::shared_ptr<SceneObject> object, std::string typeId)
{
    ASSERT(false); //TODO
    return nullptr;
}

std::shared_ptr<EGEPacket> EGEPacket::generateSSceneObjectUpdate_Main(std::shared_ptr<SceneObject> object)
{
    ASSERT(false); //TODO
    return nullptr;
}

std::shared_ptr<EGEPacket> EGEPacket::generateSSceneObjectUpdate_Extended(std::shared_ptr<SceneObject> object)
{
    ASSERT(false); //TODO
    return nullptr;
}
std::shared_ptr<EGEPacket> EGEPacket::generateSSceneObjectDeletion(long long id)
{
    ASSERT(false); //TODO
    return nullptr;
}

std::shared_ptr<EGEPacket> EGEPacket::generateSSceneCreation(std::shared_ptr<ObjectMap> userData)
{
    ASSERT(false); //TODO
    return nullptr;
}

std::shared_ptr<EGEPacket> EGEPacket::generateSSceneDeletion(std::shared_ptr<ObjectMap> userData)
{
    ASSERT(false); //TODO
    return nullptr;
}

std::shared_ptr<EGEPacket> EGEPacket::generateCSceneObjectControl(long long id, std::shared_ptr<ObjectMap> userData) //SResult
{
    ASSERT(false); //TODO
    return nullptr;
}

std::shared_ptr<EGEPacket> EGEPacket::generateSSceneObjectControl(long long id, std::shared_ptr<ObjectMap> userData)
{
    ASSERT(false); //TODO
    return nullptr;
}

}