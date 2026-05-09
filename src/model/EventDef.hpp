
#ifndef SPIRELIKE_EVENTDEF_HPP
#define SPIRELIKE_EVENTDEF_HPP

#include "Types.hpp"

#include <string>
#include <vector>

struct EventEffect {
    EventEffectType type = EventEffectType::None;
    int value = 0;
};

struct EventChoiceDef {
    std::string text;
    std::string description;
    std::vector<EventEffect> effects;
};

struct EventDef {
    EventId id = 0;
    std::string title;
    std::string description;
    std::vector<EventChoiceDef> choices;
    std::string backgroundTextureId;
};

#endif //SPIRELIKE_EVENTDEF_HPP