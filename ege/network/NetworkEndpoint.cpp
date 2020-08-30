/*
EGE - Extendable Game Engine
Copyright (c) Sppmacd 2020
*/

#include "NetworkEndpoint.h"

#include <iostream>

namespace EGE
{

bool NetworkEndpoint::send(std::shared_ptr<Packet> packet)
{
    sf::Lock lock(m_accessMutex);

    sf::Packet sfPacket = packet->toSFMLPacket();
    sf::Socket::Status status = m_socket->send(sfPacket);
    if(status != sf::Socket::Done)
    {
        std::cerr << "0017 EGE/network: Socket Send failed (system error)" << std::endl;
        disconnect();
        return false;
    }
    return true;
}

std::shared_ptr<Packet> NetworkEndpoint::receive()
{
    sf::Lock lock(m_accessMutex);

    sf::Packet sfPacket;
    sf::Socket::Status status = m_socket->receive(sfPacket);
    if(status != sf::Socket::Done)
    {
        std::cerr << "0016 EGE/network: Socket Receive failed (system error)" << std::endl;
        disconnect();
        return nullptr;
    }
    return makePacket(sfPacket);
}

void NetworkEndpoint::disconnect()
{
    sf::Lock lock(m_accessMutex);

    std::cerr << "001A EGE/network: Disconnecting network endpoint: " << m_socket->getLocalPort() << std::endl;
    m_connected = false;
    m_socket->disconnect();
}

}