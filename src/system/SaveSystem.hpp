#ifndef SPIRELIKE_SAVESYSTEM_HPP
#define SPIRELIKE_SAVESYSTEM_HPP

#include "model/RunState.hpp"

#include <string>

class SaveSystem {
public:
    static bool saveRun(
        const RunState& runState,
        const std::string& path = "save.txt"
    );

    static bool loadRun(
        RunState& runState,
        const std::string& path = "save.txt"
    );

    static bool hasSave(
        const std::string& path = "save.txt"
    );

    static bool deleteSave(
        const std::string& path = "save.txt"
    );
};

#endif
