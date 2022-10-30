#include "Spawn.h"

#include "spdlog/spdlog.h"
#include "World.h"
#include "RegularMonster.h"
#include "BeheadableMonster.h"
#include "Character.h"

#include <inix.h>
#include <inix/common.h>
#include <bango/utils/random.h>

using namespace bango::utils;

Spawn::Spawn(const std::unique_ptr<GenMonster>& init)
    :m_init(init)
{
    if(InitMonster::DB().find(GetMonsterIndex()) != InitMonster::DB().end())
        CreateSpawn();
    else
        spdlog::error("Monster ID does not exist in InitMonster {}", GetMonsterIndex()); // TODO: object shouldn't be created.

    SetNextSpawnCycle();
}

void GenMonster::set(bango::processor::lisp::var param)
{
    switch (FindAttribute(param.pop()))
    {
        case A_INDEX:       MonsterIndex = param.pop(); break;
        case A_MAP:         Map          = param.pop(); break;
        case A_AREA:        Area        = param.pop(); break;
        case A_MAX:         Amount       = param.pop(); break;
        case A_SPAWNCYCLE:  SpawnCycle   = param.pop();
                            SpawnCycle   *= 1000;
                            break;
        case A_RECT:        Rect.X1      = param.pop();
                            Rect.Y1      = param.pop();
                            Rect.X2      = param.pop();
                            Rect.Y2      = param.pop();
                            break;
    }
}

void Spawn::Tick()
{
    if((time::now() - m_next_spawn_cycle).count() > GetSpawnCycle())
        return;

    for(auto& monster : m_area_monsters)
    {
        if(monster->IsGState(CGS_KO))
            RespawnOnWorld(monster);
    }

    SetNextSpawnCycle();
}

void Spawn::RespawnOnWorld(std::shared_ptr<Monster> monster)
{
    monster->RestoreInitialState(m_init->Rect.GetRandomX(), m_init->Rect.GetRandomY());
    World::Add(monster);
}

void Spawn::CreateSpawn()
{
    const auto& init_monster = InitMonster::DB().at(GetMonsterIndex());
    m_area_monsters.resize(GetAmount());
    for (int i = 0; i < GetAmount(); i++)
    {
        auto monster = Monster::CreateMonster(GetMonsterIndex(), GetRandomX(), GetRandomY(), GetMap());
        if(!monster)
            spdlog::error("Monster was not created correctly.");
        m_area_monsters.at(i) = monster;
        World::Add(monster);
    }
}

void Spawn::SetNextSpawnCycle()
{
    m_next_spawn_cycle = (time::now() + time::duration(GetSpawnCycle()));
}

std::int32_t Spawn::GetArea() const
{
    return m_init->Area;
}

std::int32_t Spawn::GetMonsterIndex() const
{
    return m_init->MonsterIndex;
}

std::int32_t Spawn::GetMap() const
{
    return m_init->Map;
}

std::int32_t Spawn::GetAmount() const
{
    return m_init->Amount;
}

std::int32_t Spawn::GetSpawnCycle() const
{
    return m_init->SpawnCycle;
}

std::int32_t Spawn::GetRandomX() const
{
    return m_init->Rect.GetRandomX();
}

std::int32_t Spawn::GetRandomY() const
{
    return m_init->Rect.GetRandomY();
}

GenMonster::RectXY Spawn::GetRect() const
{
    return m_init->Rect;
}

std::int32_t GenMonster::RectXY::GetRandomX() const
{
    return bango::utils::random::between(X1, X2) * 32;
}

std::int32_t GenMonster::RectXY::GetRandomY() const
{
    return bango::utils::random::between(Y1, Y2) * 32;
}

unsigned int GenMonster::index() const
{
    return Area;
}