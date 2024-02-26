#include <fstream>

#include "rufs.h"

void Filesystem::write_file(Filable &file)
{
    std::ofstream fs(name, std::ios::binary | std::ios::app);
    // fs.seekp(std::ios::end); // make sure we're at end

    // shared between all 'file' types, no need to be in switch
    fs.write(file.name, sizeof(file.name));

    if (!dirStack.empty())
    {
        // Add item to current dir counter
        dirStack.top().dir.contents.set<Directory>(Directory{dirStack.top().dir.contents.get<Directory>().size + 1});
    }

    if (file.contents.is<TextFile>())
    {
        fs.write((char *)&file.contents.get<TextFile>().size, sizeof(int));
        fs.write(file.contents.get<TextFile>().data.c_str(), file.contents.get<TextFile>().data.length());
    }
    else if (file.contents.is<ProgramFile>())
    {
        fs.write((char *)&file.contents.get<ProgramFile>().cpu_req, sizeof(int));
        fs.write((char *)&file.contents.get<ProgramFile>().mem_req, sizeof(int));
    }
    else
    {
        dirStack.push({file, fs.tellp()});
        fs.write((char *)&file.contents.get<Directory>().size, sizeof(file.contents.get<Directory>().size));
    }
}

void Filesystem::create_dir(const std::string &dir_name)
{
    Filable new_dir;

    // Fill directory name with nulls
    for (char &i : new_dir.name)
    {
        i = '\0';
    }

    // copy over file name
    for (unsigned int i = 0; i < dir_name.length(); i++)
    {
        new_dir.name[i] = dir_name[i];
    }

    new_dir.name[8] = '.';
    new_dir.name[9] = 'd';
    new_dir.name[10] = '\0';

    new_dir.contents.set<Directory>(Directory{0});

    write_file(new_dir);
}

void Filesystem::end_dir()
{
    if (!dirStack.empty())
    {
        std::fstream fs(name, std::ios::binary | std::ios::out | std::ios::in | std::ios_base::ate);
        int num_contents_loc = dirStack.top().index;
        int num_contents = dirStack.top().dir.contents.get<Directory>().size;

        fs.seekp(num_contents_loc);
        fs.write((char *)&num_contents, sizeof(int));

        fs.seekg(0, std::ios::end);
        fs.write("End", 3);
        fs.write(dirStack.top().dir.name, 11);

        dirStack.pop();
    }
}

void Filesystem::close_dirs()
{
    while (!dirStack.empty())
    {
        end_dir();
    }
}

// I think this should be seperated out into seperate files, but I'm tired of writing this abomination
void Filesystem::print()
{
    std::cout << "Binary file structure is: " << std::endl;

    std::ifstream fs(name, std::ios::in | std::ios::binary);

    if (!fs.is_open())
    {
        std::cout << "Unable to open file" << std::endl;
        return;
    }

    while (fs.peek() != EOF)
    {

        if (fs.peek() == 'E')
        {
            char end_dir[14];

            int index = fs.tellg();

            fs.read(end_dir, 14);

            if (end_dir[11] == '.' && end_dir[12] == 'd')
            {
                std::cout << index << ": End of directory ";

                std::string end_name(end_dir + 3);

                if (end_name.length() < 10)
                {
                    end_name += ".d";
                }

                std::cout << end_name << std::endl;
            }
            else
            {
                fs.seekg(-14, std::ios::cur);
            }

            continue;
        }

        // std::cout << std::endl;
        std::cout << fs.tellg() << ": ";

        char name_buffer[11];
        fs.read(name_buffer, 11);

        std::string name(name_buffer);

        // I would normally format this so everything is lined up nicely between 1 and 2 digit numbers, but that is almost as painful as reading in this file
        // and I would like to retain what little of my sanity is left
        switch (name_buffer[9])
        {
        case 'd':
        {
            std::cout << "Directory: " << name + ".d" << std::endl;

            int size_start;
            size_start = fs.tellg();

            int dir_size;
            fs.read(reinterpret_cast<char *>(&dir_size), sizeof(int));

            std::cout << size_start << ": Directory " << name << " contains " << dir_size << (dir_size > 1 ? " files/directories" : " file/directory") << std::endl;
            break;
        }
        case 'p':
        {
            std::cout << "Filename: " << name + ".p" << std::endl;
            std::cout << "     Type:  Program" << std::endl;

            int cpu_req;
            int mem_req;

            fs.read(reinterpret_cast<char *>(&cpu_req), sizeof(int));
            fs.read(reinterpret_cast<char *>(&mem_req), sizeof(int));

            std::cout << "     Contents: CPU Requirement:  " << cpu_req << ", Memory Requirement:  " << mem_req << std::endl;
            break;
        }
        case 't':
        {
            std::cout << "Filename: " << name + ".t" << std::endl;
            std::cout << "     Type: Text file" << std::endl;

            int text_size;

            fs.read(reinterpret_cast<char *>(&text_size), sizeof(int));

            std::cout << fs.tellg() << ":  Size of text file: " << text_size << (text_size > 1 ? " bytes" : " byte") << std::endl;
            std::cout << fs.tellg() << ":  Contents of text file:   ";

            char text[text_size + 1];
            text[text_size] = '\0';
            fs.read(text, text_size);

            std::cout << text << std::endl;

            break;
        }
        default:
            break;
        }
    }
}
