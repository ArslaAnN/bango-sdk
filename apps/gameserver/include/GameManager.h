#pragma once

#include <bango/network.h>
#include <bango/space.h>

#include <inix/protocol.h>
#include <inix/common.h>
#include <inix/structures.h>

#include "Player.h"

using namespace bango::network;
using namespace bango::space;

class GameManager
{
private:
    tcp_server m_gameserver;
    tcp_client m_dbclient;

    map *m_map;


public:
    ~GameManager();

    void Initialize();
    bool ConnectToDatabase(const std::string& host, const std::int32_t port);
    bool StartGameServer(const std::string& host, const std::int32_t port);
};