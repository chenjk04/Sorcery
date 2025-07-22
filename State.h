#ifndef STATE_H
#define STATE_H

enum class Action {attack, use, play};
enum class Source {B5, B1, B2, B3, B4, H5, H1, H2, H3, H4};
enum class Target {player, null, P1B1, P1B2, P1B3, P1B4, P1B5, P2B1, P2B2, P2B3, P2B4, P2B5, P1BALL, P2BALL};

// board handles 4 types of triggers itself
// player handles draw, discard
// 

struct State // for manual orders during the turns
{
    Action action;
    Source source;
    Target target;
};
#endif
// start of turn trigger: game call fn of player, player call fn of board, board call 
// card->startofturn(player) for each card. 
// maybe startofturn(player) {
//      for cards: player->board, do: --card->health;}


// fn needed for each minion: they have function pointers so we can have fn do nothing using nullptr
// start of turn, own minions enters play, enemy minions enter play, enemy dies, own minion dies, end of turn..
// we can be as detailed as possible since adding new kinds of fn i just add a new data field of fn ptr

// manual notify will trigger some, passive call of player.startofturn.. will trigger others
