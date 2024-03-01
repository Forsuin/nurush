#define FMT_HEADER_ONLY

#include <string>
#include <stack>
#include <vector>
#include <list>
#include <memory>

#include "variant.h"
#include "optional.hpp"

#include <fmt/core.h>

/*
    Interface defining what a Filable element of the fs is
*/
struct Filable
{
    char name[11] = {0};

    virtual void write(std::ofstream &ofile) = 0;
    virtual void get_input() = 0;

    virtual ~Filable() = default;
};

/*
    A program file. Handles getting it's own input and writing itself to disk
*/
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

/*
    A text file. Handles getting it's own input and writing itself to disk
*/
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

        std::cout << "Enter file contents: ";
        // consume newline
        std::cin.ignore();
        getline(std::cin, input);

        data = input;
    }
};

/*
    A directory "file". Holds an array of its contents. Also holds a reference to its parent for moving up the fs
    Handles writing itself to disk
*/
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

        std::string end = "End";
        ofile.write(end.c_str(), end.length());
        ofile.write(name, 11);
    }

private:
    void get_input() {}
};

/*
    Defines the filesystme as a whole, the root node is created when a new FS is constructed.
    Also is responsible for keeping track of the current directory
*/
struct Filesystem
{
    Directory root;               // Filesystem owns root
    Directory *cur_dir = nullptr; // non-owning reference, default to nullptr
    std::string name;

    void create_file(std::shared_ptr<Filable> &file);
    void create_dir(const std::string &dir_name);
    void dotdot();
    Filable *find(std::string &filename);
    bool contains(const std::string &filename) const;
    void write();
    void close_dir();

    std::string pwd();
    std::string ls();
    bool change_dir(std::string new_dir);
    std::string cat(std::string file);
    std::string get_info();

    Filesystem(std::string name);

private:
    void read_dir(std::ifstream &ifile);
    void read_text(std::ifstream &ifile);
    void read_prog(std::ifstream &ifile);
};