
#include "CardInfo.h"

const std::unordered_map<std::string,CardInfo> CardInfo::infoMap_ = {
    // Minions 
    { "Air Elemental",       CardInfo("Air Elemental",       "Minion", 0, "",                            1, 1) },
    { "Earth Elemental",     CardInfo("Earth Elemental",     "Minion", 3, "",                            4, 4) },
    { "Bone Golem",          CardInfo("Bone Golem",          "Minion", 2, "Gain +1/+1 whenever a minion leaves play.", 1, 3) },
    { "Fire Elemental",      CardInfo("Fire Elemental",      "Minion", 2, "Whenever an opponent's minion enters play, deal 1 damage to it.", 2, 2) },
    { "Potion Seller",       CardInfo("Potion Seller",       "Minion", 2, "At the end of your turn, all your minions gain +0/+1.", 1, 3) },
    { "Novice Pyromancer",   CardInfo("Novice Pyromancer",   "Minion", 1, "Deal 1 damage to target minion", 1, 1) },
    { "Apprentice Summoner", CardInfo("Apprentice Summoner", "Minion", 1, "Summon a 1/1 air elemental",    1, 1) },
    { "Master Summoner",     CardInfo("Master Summoner",     "Minion", 3, "Summon up to three 1/1 air elementals", 2, 3) },

    // Spells :contentReference[oaicite:5]{index=5}
    { "Banish",      CardInfo("Banish",      "Spell", 2, "Destroy target minion or ritual") },
    { "Unsummon",    CardInfo("Unsummon",    "Spell", 1, "Return target minion to its owner's hand") },
    { "Recharge",    CardInfo("Recharge",    "Spell", 1, "Your ritual gains 3 charges") },
    { "Disenchant",  CardInfo("Disenchant",  "Spell", 1, "Destroy the top enchantment on target minion") },
    { "Raise Dead",  CardInfo("Raise Dead",  "Spell", 1, "Resurrect the top minion in your graveyard and set its defence to 1") },
    { "Blizzard",    CardInfo("Blizzard",    "Spell", 3, "Deal 2 damage to all minions") },

    // Enchantments :contentReference[oaicite:6]{index=6}
    { "Giant Strength",  CardInfo("Giant Strength",  "Enchantment", 1, "",  2, 2) },
    { "Enrage",          CardInfo("Enrage",          "Enchantment", 2, "",  2, 2) },
    { "Haste",           CardInfo("Haste",           "Enchantment", 1, "Enchanted minion gains +1 action each turn") },
    { "Magic Fatigue",   CardInfo("Magic Fatigue",   "Enchantment", 0, "Enchanted minion's activated ability costs 2 more") },
    { "Silence",         CardInfo("Silence",         "Enchantment", 1, "Enchanted minion cannot use abilities") },

    // Rituals :contentReference[oaicite:7]{index=7}
    { "Dark Ritual",  CardInfo("Dark Ritual",  "Ritual", 0, "At the start of your turn, gain 1 mana",       0, 0, 1, 5) },
    { "Aura of Power",CardInfo("Aura of Power","Ritual", 1, "Whenever a minion enters play under your control, it gains +1/+1", 0, 0, 1, 4) },
    { "Standstill",   CardInfo("Standstill",   "Ritual", 3, "Whenever a minion enters play, destroy it",    0, 0, 2, 4) }
};


const CardInfo& CardInfo::get(const std::string& key) {
    auto it = infoMap_.find(key);
    if (it != infoMap_.end()) return it->second;
    static const CardInfo unknown{"Unknown","Unknown",0,""};
    return unknown;
}


