#include <fstream>
#include <memory>
#include <algorithm>
#include <memory>
#include <algorithm>
#include <iterator>
#include <iterator>

#include "rufs.h"

/*
    Add a file to current working directory
*/
void Filesystem::create_file(std::shared_ptr<Filable> &file)
{
    // copies file into vector so safe to pass reference
    cur_dir->files.push_back(file);
}

/*
    Create a directory in filesytem and add it to current working directory
*/
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
}

/*
    Move working directory up to parent if parent exists, otherwise stay at root
*/
void Filesystem::dotdot()
{
    if (cur_dir->parent != nullptr)
        cur_dir = cur_dir->parent;
}

// returns file if it exists in current directory
Filable *Filesystem::find(std::string &filename)
{
    auto i = std::find_if(cur_dir->files.begin(), cur_dir->files.end(), [filename](std::shared_ptr<Filable> &f)
                          { return std::string(f.get()->name) == filename; });
    return (i != cur_dir->files.end()) ? (*i).get() : nullptr;
}

// Checks if file exists in current directory
bool Filesystem::contains(const std::string &filename) const
{
    return std::find_if(cur_dir->files.begin(), cur_dir->files.end(), [filename](std::shared_ptr<Filable> &f)
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

/*
    Read in a directory form a given ifstream and add it to filesystem
*/
void Filesystem::read_dir(std::ifstream &ifile)
{
    char name[11];
    ifile.read(name, 11);

    int size;
    ifile.read((char *)&size, sizeof(int));

    // string constructor will stop at first null
    if (std::string(name) == "root")
    {
        create_dir("root");
    }

    for (int i = 0; i < size; i++)
    {
        ifile.read(name, 11);
        ifile.seekg(-11, std::ios_base::cur);

        switch (name[9])
        {
        case 'p':
            read_prog(ifile);
            break;
        case 't':
            read_text(ifile);
            break;
        default:
            break;
        }
    }

    // skip over End directory portion as it's not needed
    ifile.seekg(14, std::ios::cur);
}

/*
    Read in a text file from a given ifstream and add to filesystem
*/
void Filesystem::read_text(std::ifstream &ifile)
{
    char name[11];
    ifile.read(name, 11);

    int size;
    ifile.read((char *)&size, sizeof(int));

    char contents[size];
    ifile.read(contents, size);

    Text t;
    t.data = contents;

    std::shared_ptr<Filable> file = std::make_shared<Text>(t);

    create_file(file);
}

/*
    Read in a program file from a given ifstream and add to filesystem
*/
void Filesystem::read_prog(std::ifstream &ifile)
{
    char name[11];
    ifile.read(name, 11);

    int cpu;
    int mem;
    ifile.read((char *)&cpu, sizeof(int));
    ifile.read((char *)&mem, sizeof(int));

    Program p;
    p.cpu = cpu;
    p.mem = mem;

    std::shared_ptr<Filable> file = std::make_shared<Program>(p);

    create_file(file);
}

/*
    Constructor
*/
Filesystem::Filesystem(std::string name) : name(name)
{
    if (std::ifstream(name).good())
    {
        std::ifstream ifile(name, std::ios::binary);

        // First thing is all filesystems is root
        read_dir(ifile);
        return;
    }

    create_dir("root");
}

/*
    Print the abosulute path to the current working directory
*/
std::string Filesystem::pwd()
{
    std::string output = "";

    Directory *current_dir = cur_dir;

    while (current_dir != nullptr)
    {
        if (output == "")
        {
            output = fmt::format("{}", current_dir->name);
        }
        else
        {
            output = fmt::format("{}/{}", current_dir->name, output);
        }

        current_dir = current_dir->parent;
    }

    return output;
}

/*
    List what directory your in and all the contents of that directory
*/
std::string Filesystem::ls()
{
    std::string output;

    output += fmt::format("Directory Name: {}\n", cur_dir->name);

    for (auto &f : cur_dir->files)
    {
        if (Text *t = dynamic_cast<Text *>(f.get()))
        {
            output += fmt::format("Filename: {} Type: Text file\n", t->name);
        }
        else if (Program *p = dynamic_cast<Program *>(f.get()))
        {
            output += fmt::format("Filename: {} Type: Program file\n", p->name);
        }
        else
        {
            Directory *d = dynamic_cast<Directory *>(f.get());

            output += fmt::format("Filename: {} Type: Directory\n", d->name);
        }
    }

    return output;
}

/*
    Change working directory to new_dir and return if change was possible
*/
bool Filesystem::change_dir(std::string new_dir)
{
    if (!contains(new_dir))
    {
        return false;
    }

    Filable *sub_dir = find(new_dir);

    if (Directory *d = dynamic_cast<Directory *>(sub_dir))
    {
        cur_dir = d;
        return true;
    }
    else
    {
        return false;
    }
}

/*
    Prints the contents of a text file
*/
std::string Filesystem::cat(std::string file)
{
    if (!contains(file))
    {
        return "File does not exist";
    }

    Filable *f = find(file);

    if (Text *t = dynamic_cast<Text *>(f))
    {
        return fmt::format("Text file contents:\n{}", t->data);
    }
    else
    {
        return "Invalid file type";
    }
}

void read_text_info(std::ifstream &ifile, std::string &output)
{

    char name[11];
    ifile.read(name, 11);

    output += fmt::format("{}: Filename: {}.t\nType: Text file\n", (int)ifile.tellg(), std::string(name));

    int size;
    ifile.read((char *)&size, sizeof(int));

    output += fmt::format("{}: Size of text file: {} byte{}\n", (int)ifile.tellg(), size, (size > 1) ? "s" : "");

    char contents[size];
    ifile.read(contents, size);

    output += fmt::format("{}: Contents of text file: {}\n", (int)ifile.tellg(), std::string(contents));
}

void read_prog_info(std::ifstream &ifile, std::string &output)
{
    char name[11];
    ifile.read(name, 11);

    output += fmt::format("{}: Filename: {}.p\nType: Program\n", (int)ifile.tellg(), std::string(name));

    int cpu;
    int mem;
    ifile.read((char *)&cpu, sizeof(int));
    ifile.read((char *)&mem, sizeof(int));

    output += fmt::format("Contents: CPU Requirement: {}, Memory Requirement: {}\n", cpu, mem);
}

void read_dir_info(std::ifstream &ifile, std::string &output)
{
    output += fmt::format("{}: Directory: ", (int)ifile.tellg());

    char name[11];
    ifile.read(name, 11);

    output += fmt::format("{}.d\n", std::string(name));

    int size;
    ifile.read((char *)&size, sizeof(int));

    output += fmt::format("{}: Directory {} contains {} {}\n", (int)ifile.tellg(), std::string(name), size, ((size == 1) ? "file/directory" : "files/directories"));

    for (int i = 0; i < size; i++)
    {
        ifile.read(name, 11);
        ifile.seekg(-11, std::ios_base::cur);

        switch (name[9])
        {
        case 'p':
            read_prog_info(ifile, output);
            break;
        case 't':
            read_text_info(ifile, output);
            break;
        default:
            break;
        }
    }

    // skip over End directory portion as it's not needed
    ifile.seekg(14, std::ios::cur);

    output += fmt::format("{}: End of directory {}.d\n", (int)ifile.tellg(), std::string(name));
}

std::string Filesystem::get_info()
{
    // Write current state of fileystem to disk
    write();

    std::ifstream ifile(name, std::ios::binary);
    std::string output;

    read_prog_info(ifile, output);

    return output;
}

/*
    void Filesystem::read_dir(std::ifstream &ifile)
{
    char name[11];
    ifile.read(name, 11);

    int size;
    ifile.read((char *)&size, sizeof(int));

    // string constructor will stop at first null
    if (std::string(name) == "root")
    {
        create_dir("root");
    }

    for (int i = 0; i < size; i++)
    {
        ifile.read(name, 11);
        ifile.seekg(-11, std::ios_base::cur);

        switch (name[9])
        {
        case 'p':
            read_prog(ifile);
            break;
        case 't':
            read_text(ifile);
            break;
        default:
            break;
        }
    }

    // skip over End directory portion as it's not needed
    ifile.seekg(14, std::ios::cur);
}

void Filesystem::read_text(std::ifstream &ifile)
{
    char name[11];
    ifile.read(name, 11);

    int size;
    ifile.read((char *)&size, sizeof(int));

    char contents[size];
    ifile.read(contents, size);

    Text t;
    t.data = contents;

    std::shared_ptr<Filable> file = std::make_shared<Text>(t);

    create_file(file);
}

void Filesystem::read_prog(std::ifstream &ifile)
{
    char name[11];
    ifile.read(name, 11);

    int cpu;
    int mem;
    ifile.read((char *)&cpu, sizeof(int));
    ifile.read((char *)&mem, sizeof(int));

    Program p;
    p.cpu = cpu;
    p.mem = mem;

    std::shared_ptr<Filable> file = std::make_shared<Program>(p);

    create_file(file);
}
*/

// std::string Filesystem::get_info()
// {
// /*
//     Name: 11 bytes

//     Text: int, n bytes
//     Program: int, int
//     Directory: int, n bytes, EndName (14 bytes)
// */

//     std::ifstream ifile(name, std::ios::binary);

//     std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(ifile)),
//                                       std::istreambuf_iterator<char>());

//     std::string output;
//     int byte_index = 0;

//     for (int it = 0; it < buffer.size();)
//     {
//         char name[11];

//         for (int i = 0; i < 11; i++, it++)
//         {
//             name[i] = buffer[it];
//         }

//         fmt::println("{}", name);

//         switch (name[9])
//         {
//         case 'd':
//         {
//             int size =
//                 break;
//         }

//         default:
//             break;
//         }
//     }

//     return output;
// }