#include "DataLoader.hpp"
#include <fstream>
#include <sstream>
#include <cctype>

#include "core/Logger.hpp"

ErrorCode DataLoader::readCsv(
    const std::string& path,
    std::vector<std::vector<std::string>>& rows
)
{
    rows.clear();

    std::ifstream file(path);

    if (!file.is_open()) {
        LOG_ERROR("CSV file not found: path=" << path);
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
    std::string current;
    bool inQuotes = false;

    for (std::size_t i = 0; i < line.size(); ++i)
    {
        char c = line[i];

        if (c == '"')
        {
            if (
                inQuotes &&
                i + 1 < line.size() &&
                line[i + 1] == '"'
            )
            {
                current += '"';
                ++i;
            }
            else
            {
                inQuotes = !inQuotes;
            }
        }
        else if (c == delimiter && !inQuotes)
        {
            result.push_back(trim(current));
            current.clear();
        }
        else
        {
            current += c;
        }
    }

    result.push_back(trim(current));

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
