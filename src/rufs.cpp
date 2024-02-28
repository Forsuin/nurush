#include <fstream>
#include <memory>
#include <algorithm>
#include <memory>
#include <algorithm>

#include "rufs.h"

void Filesystem::create_file(Filable &file)
{
    cur_dir->contents.get<Directory>().files.push_back(std::make_shared<Filable>(file));
}

void Filesystem::create_dir(const std::string &dir_name)
{
    Filable new_dir;
    new_dir.contents.set<Directory>();

    for (int i = 0; i < dir_name.length(); i++)
    {
        new_dir.name[i] = dir_name[i];
    }

    new_dir.name[8] = '.';
    new_dir.name[9] = 'd';
    new_dir.name[10] = '\0';

    if (dir_name == "root")
    {
        root = new_dir;
        cur_dir = &root;
        return;
    }

    new_dir.contents.get<Directory>().parent = cur_dir;
    cur_dir->contents.get<Directory>().files.push_back(std::make_shared<Filable>(new_dir));
    cur_dir = (*(cur_dir->contents.get<Directory>().files.end())).get();
}

void Filesystem::dotdot()
{
    cur_dir = cur_dir->contents.get<Directory>().parent;
}

// returns file if it exists in current directory
const tl::optional<Filable &> Filesystem::find(const std::string &filename) const
{
    auto i = std::find_if(cur_dir->contents.get<Directory>().files.begin(), cur_dir->contents.get<Directory>().files.end(), [filename](std::shared_ptr<Filable> &f)
                          { return std::string(f.get()->name) == filename; });

    return (i != cur_dir->contents.get<Directory>().files.end()) ? tl::make_optional<Filable &>(*(*i).get()) : tl::nullopt;
}

// Checks if file exists in current directory
bool Filesystem::contains(const std::string &filename) const
{
    return std::find_if(cur_dir->contents.get<Directory>().files.begin(), cur_dir->contents.get<Directory>().files.end(), [filename](std::shared_ptr<Filable> &f)
                        { return std::string(f.get()->name) == filename; }) != cur_dir->contents.get<Directory>().files.end();
};

// Write filesystem to "disk" file
void Filesystem::write()
{
    std::ofstream ofile(name, std::ios::binary);

    Directory *current = &root.contents.get<Directory>();
}

// Closes a directory a moves up to the parent directory
void Filesystem::close_dir()
{
    if (cur_dir->contents.get<Directory>().parent != nullptr)
    {
        cur_dir = cur_dir->contents.get<Directory>().parent;
    }
}