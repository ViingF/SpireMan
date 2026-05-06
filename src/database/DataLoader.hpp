
#ifndef SPIRELIKE_DATALOADER_HPP
#define SPIRELIKE_DATALOADER_HPP
#include "model/Types.hpp"

class DataLoader {
public:
    static ErrorCode readCsv(
        const std::string& path,
        std::vector<std::vector<std::string>>& rows
    );

private:
    static std::vector<std::string> splitLine(
        const std::string& line,
        char delimiter
    );

    static std::string trim(
        const std::string& text
    );
};
#endif //SPIRELIKE_DATALOADER_HPP