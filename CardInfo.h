// CardInfo.h

#include <string>
#include <unordered_map>

class CardInfo {
public:
    // full constructor (you can supply only the fields you need)
    CardInfo(std::string name = "", int cost = 0, std::string desc = "", std::string type = "", int atk = 0, int def = 0, int actCost = 0)
      : name_(std::move(name))
      , cost_(cost)
      , desc_(std::move(desc))
      , type_(std::move(type))
      , atk_(atk)
      , def_(def)
      , actCost_(actCost)
      //, charges_(charges)
    {}

    // getters
    const std::string& name() const { return name_; }
    int cost() const { return cost_; }
    const std::string& description() const { return desc_;        }
    const std::string& type() const { return type_;        }
    int attack() const { return atk_;  }
    int                 health()      const { return hp_;          }
    int                 activatedCost() const { return actCost_;    }
    int                 charges()     const { return charges_;     }

    // lookup by card name
    static const CardInfo& get(const std::string& cardName);

private:
    std::string name_;
    int         cost_;
    std::string desc_;
    int         atk_, hp_;
    int         actCost_, charges_;

    // central hard‑coded database
    static const std::unordered_map<std::string,CardInfo> infoMap_;
};
