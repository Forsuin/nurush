#define FMT_HEADER_ONLY

#include <string>
#include <stack>
#include <vector>
#include <list>
#include <memory>

#include "variant.h"
#include "optional.hpp"

#include <fmt/core.h>

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
    std::vector<std::shared_ptr<Filable>> files;
    Filable *parent;
};

struct Filable
{
    char name[11] = {0};

    variant<Program, Text, Directory> contents;
};

struct Filesystem
{
    Filable root;               // Filesystem owns root
    Filable *cur_dir = nullptr; // non-owning reference, default to nullptr
    std::string name;

    void create_file(Filable &file);
    void create_dir(const std::string &dir_name);
    void dotdot();
    const tl::optional<Filable &> find(const std::string &filename) const;
    bool contains(const std::string &filename) const;
    void write();
    void close_dir();

    void print();

    Filesystem() : name("rufs.dat")
    {
        create_dir("root");
    }
    Filesystem(std::string name) : name(name) {}
};