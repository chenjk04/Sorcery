
#include "CardInfo.h"

// 1) fill in every card you need here:
//(std::string name = "", int cost = 0, std::string desc = "", int atk = 0, int def = 0, int actCost = 0)
const std::unordered_map<std::string,CardInfo> CardInfo::infoMap_ = {
    
    { "Minion",    CardInfo("Air Elemental", 0, "",1, 1) },
    { "Minion",    CardInfo("Earth Elemental",    3, "", 4, 4) },
    { "Minion",    CardInfo("Bone Golem",         2, "Gain +1/+1 whenever a minion leaves play.", 1, 3) },
    { "Minion",    CardInfo("Fire Elemental",     2, "Whenever an opponent’s minion enters play, deal 1 damage to it.", 2, 2) },
    { "Minion",    CardInfo("Potion Seller",      2, "At the end of your turn, all your minions gain +0/+1.", 1, 3) },
    { "Minion",  CardInfo("Novice Pyromancer",  1, "Deal 1 damage to target minion.", 0, 1) },
    { "Minion",CardInfo("Apprentice Summoner",1, "Summon a 1/1 air elemental.", 1, 1) },
    { "Minion",    CardInfo("Master Summoner",    3, "Summon up to three 1/1 air elementals.", 2, 3) },
    
    { "Spell",      CardInfo("Banish",      2, "Destroy target minion or ritual") },
    { "Spell",    CardInfo("Unsummon",    1, "Return target minion to its owner's hand") },
    { "Spell",    CardInfo("Recharge",    1, "Your ritual gains 3 charges") },
    { "Spell",  CardInfo("Disenchant",  1, "Destroy the top enchantment on target minion") },
    { "Spell",  CardInfo("Raise Dead",  1, "Resurrect the top minion in your graveyard and set its defence to 1") },
    { "Spell",    CardInfo("Blizzard",    3, "Deal 2 damage to all minions") },

    { "Ritual", CardInfo("Dark Ritual", 0, "At the start of your turn, gain 1 magic", 0, 0, 1) },
    { "Ritual", CardInfo("Aura of Power", 1, "Whenever a minion enters play under your control, it gains +1/+1", 0, 0, 1) },
    { "Ritual", CardInfo("Standstill",    3, "Whenever a minion enters play, destroy it",   0, 0, 2) },

    { "Enchantment",  CardInfo("Giant Strength", 1, "", 2, 2) },
    { "Enchantment",  CardInfo("Enrage",         2, "Double enchanted minion's attack and defence", 0, 0) },
    { "Enchantment",  CardInfo("Haste",          1, "Enchanted minion gains +1 action each turn", 0, 0) },
    { "Enchantment",  CardInfo("Magic Fatigue",  0, "Enchanted minion's activated ability costs 2 more", 0, 0) },
    { "Enchantment",  CardInfo("Silence",        1, "Enchanted minion cannot use abilities", 0, 0) }


    
};

// 2) lookup function
const CardInfo& CardInfo::get(const std::string& cardName) {
    auto it = infoMap_.find(cardName);
    if (it != infoMap_.end()) return it->second;
    // fallback “unknown” card
    static const CardInfo unknown("Unknown",0,"");
    return unknown;
}


