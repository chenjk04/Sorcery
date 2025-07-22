#ifndef STATE_H
#define STATE_H
enum class Timing {start, during, end};
enum class Action {attack, use, play};
enum class Source {B5, B1, B2, B3, B4, H5, H1, H2, H3, H4};
enum class Target {player, P1B1, P1B2, P1B3, P1B4, P1B5, P2B1, P2B2, P2B3, P2B4, P2B5};

// board handles 4 types of triggers itself
// player handles draw, discard
// 

struct State
{
    Timing timimg;
    Action action;
    Source source;
    Target target;
};
#endif
