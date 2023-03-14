#pragma once

#include <map>

#include "Inventory.h"
#include "Character.h"
#include "spdlog/spdlog.h"

#include <bango/processor/db.h>
#include <inix/attributes.h>


struct LootInfo
{
    LootInfo() = default;
    LootInfo(std::uint32_t index, std::uint32_t chance, std::uint32_t amount, std::uint32_t prefix)
        : m_index(index)
        , m_chance(chance)
        , m_amount(amount)
        , m_prefix(prefix)
    {}

    constexpr bool operator==(const LootInfo&) const = default;

    std::uint32_t m_index;
    std::uint32_t m_chance;
    std::uint32_t m_amount;
    std::uint32_t m_prefix;
};

struct GroupInfo
{
    GroupInfo() = default;
    GroupInfo(std::uint32_t index, std::uint32_t chance)
        : m_index(index)
        , m_chance(chance)
    {}

    constexpr bool operator==(const GroupInfo&) const = default;


    std::uint32_t m_index;
    std::uint32_t m_chance;
};

struct Group : public bango::processor::db_object<Group>
{
    std::uint32_t m_index;
    std::map<std::uint32_t, LootInfo> m_loots_map;

    std::uint32_t GetMaxMapKey() const { return m_loots_map.empty() ? 0 : m_loots_map.rbegin()->first;}

    unsigned int index() const { return m_index; }

    void ValidateLootInfo(LootInfo current) const;
    void AssignGroup(std::vector<uint32_t> values_from_bracket);
    std::map<std::uint32_t, LootInfo>::const_iterator RollLoot() const;

    virtual void set(bango::processor::lisp::var param) override;
};


struct ItemGroup : public bango::processor::db_object<ItemGroup>
{
    std::uint32_t m_index;
    std::map<std::uint32_t, GroupInfo> m_groups_map;

    std::uint32_t GetMaxMapKey() const { return m_groups_map.empty() ? 0 : m_groups_map.rbegin()->first;}
    
    unsigned int index() const { return m_index; }

    void ValidateGroupInfo(GroupInfo current) const;
    void AssignItemGroup(std::vector<uint32_t> values_from_bracket);
    const Group* RollGroup() const;

    virtual void set(bango::processor::lisp::var param) override;
};

std::vector<std::uint32_t> GetValuesFromBrackets(bango::processor::lisp::var& param);
