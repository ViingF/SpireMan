#include "DataLoader.hpp"
#include <fstream>
#include <sstream>
#include <cctype>

ErrorCode DataLoader::readCsv(
    const std::string& path,
    std::vector<std::vector<std::string>>& rows
)
{
    rows.clear();

    std::ifstream file(path);

    if (!file.is_open())
    {
        return ErrorCode::FILE_NOT_FOUND;
    }

    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        rows.push_back(
            splitLine(line, ',')
        );
    }

    file.close();

    return ErrorCode::OK;
}

std::vector<std::string> DataLoader::splitLine(
    const std::string& line,
    char delimiter
)
{
    std::vector<std::string> result;

    std::stringstream ss(line);

    std::string item;

    while (std::getline(ss, item, delimiter))
    {
        result.push_back(
            trim(item)
        );
    }

    return result;
}

std::string DataLoader::trim(
    const std::string& text
)
{
    size_t start = 0;

    while (
        start < text.size() &&
        std::isspace(
            static_cast<unsigned char>(text[start])
        )
    )
    {
        start++;
    }

    size_t end = text.size();

    while (
        end > start &&
        std::isspace(
            static_cast<unsigned char>(text[end - 1])
        )
    )
    {
        end--;
    }

    return text.substr(start, end - start);
}
