
#ifndef SPIRELIKE_EVENTDATABASE_HPP
#define SPIRELIKE_EVENTDATABASE_HPP

#include "model/EventDef.hpp"

#include <random>
#include <unordered_map>
#include <vector>

class EventDatabase {
public:
    ErrorCode loadFromCsv(const std::string& path);

    bool exists(EventId id) const;
    const EventDef& get(EventId id) const;

    std::vector<EventId> getAllEventIds() const;

    EventId chooseRandomEventId(std::mt19937& rng) const;

private:
    std::unordered_map<EventId, EventDef> events_;
};

#endif //SPIRELIKE_EVENTDATABASE_HPP