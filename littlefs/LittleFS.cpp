#include "LittleFS.hpp"

#include <utility>


std::vector<LittleFS::FileInfo> LittleFS::recursive_dirlist(std::string const & path)
{
    std::vector<FileInfo> result {};

    std::vector<std::string> to_visit {path == "/" ? "" : path};
    while (!to_visit.empty())
    {
        auto const current_path = to_visit.back();
        to_visit.pop_back();

        for (auto const & entry : list_directory(current_path))
        {
            if (entry.name == "." || entry.name == "..")
            {
                continue;
            }

            auto entry_path = current_path + "/" + entry.name;

            if (entry.is_directory)
            {
                to_visit.push_back(std::move(entry_path));
            }
            else
            {
                result.push_back({std::move(entry_path), entry.size});
            }
        }
    }

    return result;
}
