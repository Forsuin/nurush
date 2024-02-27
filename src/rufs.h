#include <string>
#include <stack>
#include <vector>
#include <list>

#include "variant.h"

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
    std::vector<Filable> contents;

    Directory *parent;

    Directory() : parent(nullptr) {}
    Directory(Directory &parent) : parent(&parent) {}
};

struct Filable
{
    char name[11];

    variant<Program, Text, Directory> contents;
};

struct Filesystem
{
    Directory root;
    std::string name;

    void write_file(Filable &file);
    void create_dir(const std::string &dir_name);
    void end_dir();
    void close_dirs();
    void print();

    Filesystem() {}
    Filesystem(std::string name) : name(name) {}
};