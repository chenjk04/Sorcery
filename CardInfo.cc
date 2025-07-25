
#include "CardInfo.h"

// 1) fill in every card you need here:
//(std::string name = "", int cost = 0, std::string desc = "", int atk = 0, int def = 0, int actCost = 0)
const std::unordered_map<std::string,CardInfo> CardInfo::infoMap_ = {
    { "Minion", // card type
        CardInfo("Novice Pyromancer", // card name
                 1, // cost (not all of them)
                 "A mischievous creature", // desc
                 0) }, // attack / defense / attack cost
    { "Ritual",
        CardInfo("Banish",
                 2,
                 "Destroy target minion or ritual") },
    { "Spell",
        CardInfo("Silence",
                 1,
                 "Enchanted minion cannot use abilities") },
    { "Minion",
        CardInfo("Dark Ritual",
                 0,
                 "At start of your turn, gain 1 magic"
                 )}
    // add cards
};

// 2) lookup function
const CardInfo& CardInfo::get(const std::string& cardName) {
    auto it = infoMap_.find(cardName);
    if (it != infoMap_.end()) return it->second;
    // fallback “unknown” card
    static const CardInfo unknown("Unknown",0,"");
    return unknown;
}


