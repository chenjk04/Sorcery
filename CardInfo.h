
#include <string>
#include <unordered_map>

struct CardInfo {
    std::string name, type, desc;
    int cost = 0, atk = 0, hp = 0, actCost = 0, charges = 0;

        CardInfo(std::string n, std::string t, int c, std::string d,
             int a=0, int h=0, int ac=0, int ch=0)
      : name(std::move(n)), type(std::move(t)), desc(std::move(d)),  // desc moved here
        cost(c), atk(a), hp(h), actCost(ac), charges(ch)           // cost moved here
    {}

    static const CardInfo& get(const std::string& key);

private:
    static const std::unordered_map<std::string,CardInfo> infoMap_;
};

