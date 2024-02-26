

#include <string>
#include <stack>

#include "variant.h"

struct ProgramFile
{
    int cpu_req;
    int mem_req;
};

struct TextFile
{
    std::string data;
    int size;
};

struct Directory
{
    int size;
};

struct Filable
{
    char name[11];

    variant<ProgramFile, TextFile, Directory> contents;
};

// For keeping track of where in binary file directory content count is
struct DirIndexPair
{
    Filable dir;
    int index;
};

struct Filesystem
{
private:
    std::stack<DirIndexPair> dirStack;

public:
    std::string name;

    /*
    Writes a file into filesystem
*/
    void write_file(Filable &file);

    /*
        Creates a new directory of the filesystem
    */
    void create_dir(const std::string &dir_name);

    /*
        Closes most recent directory
    */
    void end_dir();

    /*
        Closes any directories that are still open
    */
    void close_dirs();

    /*
        Print out the contents of the filesystem
    */
    void print();

    Filesystem(const std::string &name) : name(name) {}
};