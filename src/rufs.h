#define FMT_HEADER_ONLY

#include <string>
#include <stack>
#include <vector>
#include <list>
#include <memory>

#include "variant.h"
#include "optional.hpp"

#include <fmt/core.h>

struct Filable
{
    char name[11] = {0};

    virtual void write(std::ofstream &ofile) = 0;
    virtual void get_input() = 0;

    virtual ~Filable() = default;
};

struct Program : virtual public Filable
{
    int cpu;
    int mem;

    void write(std::ofstream &ofile)
    {
        ofile.write(name, 11);

        ofile.write((char *)&cpu, sizeof(int));
        ofile.write((char *)&mem, sizeof(int));
    }

    void get_input()
    {
        int mem_req;
        int cpu_req;

        std::cout << "Enter CPU requirements: ";
        std::cin >> cpu_req;

        std::cout << "Enter memory requirements: ";
        std::cin >> mem_req;

        cpu = cpu_req;
        mem = mem_req;
    }
};

struct Text : virtual public Filable
{
    std::string data;

    void write(std::ofstream &ofile)
    {
        ofile.write(name, 11);

        int length = data.length();
        ofile.write((char *)&length, sizeof(int));
        ofile.write(data.c_str(), length);
    }

    void get_input()
    {
        std::string input;

        std::cout << "Enter contents: ";
        // consume newline
        std::cin.ignore();
        getline(std::cin, input);

        data = input;
    }
};

struct Directory : virtual public Filable
{
    std::vector<std::shared_ptr<Filable>> files;
    Directory *parent = nullptr;

    void write(std::ofstream &ofile)
    {
        ofile.write(name, 11);

        int num_contents = files.size();
        ofile.write((char *)&num_contents, sizeof(int));

        for (auto &f : files)
        {
            f->write(ofile);
        }

        std::string end = "End" + std::string(name);
        ofile.write(end.c_str(), end.length());
    }

private:
    void get_input() {}
};

struct Filesystem
{
    Directory root;               // Filesystem owns root
    Directory *cur_dir = nullptr; // non-owning reference, default to nullptr
    std::string name;

    void create_file(std::shared_ptr<Filable> &file);
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
    Filesystem(std::string name) : name(name)
    {
        create_dir("root");
    }
};