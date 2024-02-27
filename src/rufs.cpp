#include <fstream>
#include <memory>
#include <algorithm>

#include "rufs.h"

void Filesystem::create_file(Filable &file)
{
    cur_dir->files.push_back(file);
}

void Filesystem::create_dir(const std::string &dir_name)
{
    Directory new_dir;

    for (int i = 0; i < dir_name.length(); i++)
    {
        new_dir.name[i] = dir_name[i];
    }

    new_dir.name[8] = '.';
    new_dir.name[9] = 'd';
    new_dir.name[10] = '\0';

    new_dir.parent = cur_dir;

    cur_dir->sub_dirs.push_back(std::unique_ptr<Directory>(&new_dir));

    cur_dir = cur_dir->sub_dirs.back().get();
}

void Filesystem::dotdot()
{
    cur_dir = cur_dir->parent;
}

// returns file if it exists in current directory
const tl::optional<Filable &> Filesystem::find(const std::string &filename) const {

};

// Checks if file exists in current directory
bool Filesystem::contains(const std::string &filename) const
{
    return std::find(cur_dir->files.begin(), cur_dir->files.end(), filename) != cur_dir->files.end();
};