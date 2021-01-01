/*
EGE - Extendable Game Engine
Copyright (c) Sppmacd 2020
*/

#include "EGEPacket.h"

#include "EGEPacketConverter.h"

#include <cstdlib>
#include <ctime>
#include <ege/util/Converter.h>
#include <ege/util/ObjectInt.h>
#include <ege/util/Random.h>
#include <ege/util/system.h>
#include <SFML/Network.hpp>

namespace EGE
{

std::string EGEPacket::typeString(Type type)
{
    switch(type)
    {
        case EGEPacket::Type::_Data: return "_Data";
        case EGEPacket::Type::_Ping: return "_Ping";
        case EGEPacket::Type::_Pong: return "_Pong";
        case EGEPacket::Type::_ProtocolVersion: return "_ProtocolVersion";
        case EGEPacket::Type::SResult: return "SResult";
        case EGEPacket::Type::CLogin: return "CLogin";
        case EGEPacket::Type::SLoginRequest: return "SLoginRequest";
        case EGEPacket::Type::SDisconnectReason: return "SDisconnectReason";
        case EGEPacket::Type::SSceneObjectCreation: return "SSceneObjectCreation";
        case EGEPacket::Type::SSceneObjectUpdate: return "SSceneObjectUpdate";
        case EGEPacket::Type::SSceneObjectDeletion: return "SSceneObjectDeletion";
        case EGEPacket::Type::SSceneCreation: return "SSceneCreation";
        case EGEPacket::Type::SSceneDeletion: return "SSceneDeletion";
        case EGEPacket::Type::CSceneObjectControl: return "CSceneObjectControl";
        case EGEPacket::Type::SDefaultControllerId: return "SDefaultControllerId";
        case EGEPacket::Type::CSceneObjectRequest: return "CSceneObjectRequest";
        case EGEPacket::Type::SSceneObjectControl: return "SSceneObjectControl";
        case EGEPacket::Type::_Version: return "_Version";
        default: return "<unknown>";
    }
}

bool EGEPacket::fromSFMLPacket(sf::Packet& packet)
{
    bool success = true;
    success &= !packet.endOfPacket();
    packet >> (unsigned int&)m_type;
    success &= !packet.endOfPacket();
    std::shared_ptr<Object> args = make<ObjectMap>();
    if(!(packet >> objectIn(args, EGEPacketConverter())))
        return false;
    m_args = std::dynamic_pointer_cast<ObjectMap>(args);
    ASSERT(m_args);
    return true;
}

sf::Packet EGEPacket::toSFMLPacket()
{
    sf::Packet packet;
    packet << (unsigned int)m_type;

    if(m_args)
        EGEPacketConverter().out(packet, *m_args);

    return packet;
}

long long EGEPacket::generateUID()
{
    static EGE::Random random(EGE::System::unixTime());
    static EGE::UidType lastUID = 0;
    lastUID += random.nextIntRanged(1, 4);
    return lastUID;
}

void EGEPacket::appendUID(std::shared_ptr<ObjectMap> packetArgs)
{
    packetArgs->addObject("uid", make<ObjectInt>(generateUID()));
}

}
