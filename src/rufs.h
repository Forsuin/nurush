#include <string>
#include <stack>
#include <vector>
#include <list>
#include <memory>

#include "variant.h"
#include "optional.hpp"

struct Program
{
    int cpu;
    int mem;
};

struct Text
{
    std::string data;
};

struct Filable;
struct Directory;

struct Directory
{
    char name[11] = {0};

    std::vector<Filable> files;
    std::vector<std::unique_ptr<Directory>> sub_dirs;

    Directory *parent;
};

struct Filable
{
    char name[11] = {0};

    variant<Program, Text> contents;
};

struct Filesystem
{
    Directory root;     // Filesystem owns root
    Directory *cur_dir; // non-owning reference
    std::string name;

    void create_file(Filable &file);
    void create_dir(const std::string &dir_name);
    void dotdot();
    const tl::optional<Filable &> find(const std::string &filename) const;
    bool contains(const std::string &filename) const;

    void print();

    Filesystem() {}
    Filesystem(std::string name) : name(name) {}
};