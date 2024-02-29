#include <fstream>
#include <memory>
#include <algorithm>
#include <memory>
#include <algorithm>

#include "rufs.h"

void Filesystem::create_file(std::shared_ptr<Filable> &file)
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

    if (dir_name == "root")
    {
        root = new_dir;
        cur_dir = &root;
        return;
    }

    new_dir.parent = cur_dir;
    std::shared_ptr<Filable> ptr = std::make_shared<Directory>(new_dir);
    create_file(ptr);

    // I can pretty safely cast like this and assume it's a directory since I just added it
    cur_dir = dynamic_cast<Directory *>(cur_dir->files[cur_dir->files.size() - 1].get());
}

void Filesystem::dotdot()
{
    cur_dir = cur_dir->parent;
}

// returns file if it exists in current directory
const tl::optional<Filable &> Filesystem::find(const std::string &filename) const
{
    auto i = std::find_if(cur_dir->files.begin(), cur_dir->files.end(), [filename](std::shared_ptr<Filable> &f)
                          { return std::string(f.get()->name) == filename; });

    return (i != cur_dir->files.end()) ? tl::make_optional<Filable &>(*((*i).get())) : tl::nullopt;
}

// Checks if file exists in current directory
bool Filesystem::contains(const std::string &filename) const
{
    auto i = std::find_if(cur_dir->files.begin(), cur_dir->files.end(), [filename](std::shared_ptr<Filable> &f)
                          { return std::string(f.get()->name) == filename; }) != cur_dir->files.end();
};

// Write filesystem to "disk" file
void Filesystem::write()
{
    std::ofstream ofile(name, std::ios::binary);

    root.write(ofile);

    ofile.close();
}

// Closes a directory a moves up to the parent directory
void Filesystem::close_dir()
{
    if (cur_dir->parent != nullptr)
    {
        cur_dir = cur_dir->parent;
    }
}