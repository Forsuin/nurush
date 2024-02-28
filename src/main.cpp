#define FMT_HEADER_ONLY

#include <iostream>
#include <fstream>

#include <fmt/core.h>

#include "rufs.h"

int main(int argc, char **argv)
{

    if (argc != 2)
    {
        std::cout << "Usage: rufs [filename]" << std::endl;
        return 1;
    }

    Filesystem fs(std::string(argv[1]) + ".bin");

    // Create filesystem if it doesn't already exist,
    // otherwise we'll overwrite the filesystem
    std::ofstream(fs.name, std::ios::trunc).close();

    // Create root directory as first entry
    std::string root = "root";
    fs.create_dir(root);

    std::cout << "Welcome to RUFS. Enter one of the following commands: " << std::endl;
    std::cout << "CreateDir, CreateFile, EndDir, or quit" << std::endl;

    bool close = false;

    // Main REPL loop
    std::string input;
    while (!close)
    {
        std::cout << "Command> ";
        std::cin >> input;

        if (input == "CreateDir")
        {
            std::cout << "Enter directory name: ";
            std::cin >> input;

            bool bad_dirname = false;

            do
            {
                // reset each loop
                bad_dirname = false;

                if (input.length() > 8)
                {
                    std::cout << "Invalid directory, can only be 8 characters long" << std::endl;
                    bad_dirname = true;

                    std::cout << "Enter directory name: ";
                    std::cin >> input;
                }
            } while (bad_dirname);

            fs.create_dir(input);
        }
        else if (input == "CreateFile")
        {
            bool bad_filename = false;

            // Verify file name
            do
            {
                // reset each loop
                bad_filename = false;
                input.clear();

                std::cout << "Enter filename: ";
                std::cin >> input;

                // 10 because null character at end
                if (input.length() > 10)
                {
                    std::cout << "Invalid filename, must be 10 characters long or less" << std::endl;
                    bad_filename = true;
                }
                else if (input[input.length() - 2] != '.')
                {
                    std::cout << "Missing file extension" << std::endl;
                    bad_filename = true;
                }
                else if (input.back() != 't' && input.back() != 'p')
                {
                    std::cout << "Invalid file extension" << std::endl;
                    bad_filename = true;
                }
            } while (bad_filename);

            // Good filename now, guaranteed to be text or program

            std::shared_ptr<Filable> file;

            if (input.back() == 't')
            {
                file = std::make_shared<Text>();
                file->name[9] = 't';
            }
            else
            {
                file = std::make_shared<Program>();
                file->name[9] = 'p';
            }

            // Getting filename
            for (int i = 0;; i++)
            {
                if (input[i] == '.')
                {
                    break;
                }
                file->name[i] = input[i];
            }
            // Set extension
            file->name[8] = '.';

            file->name[10] = '\0';

            // Get input for file
            file->get_input();

            fs.create_file(file);
        }
        else if (input == "EndDir")
        {
            fs.close_dir();
        }
        else if (input == "quit")
        {

            while (fs.cur_dir->parent != nullptr)
            {
                fs.close_dir();
            }

            fs.write();

            // fs.print();

            close = true;
        }
        else
        {
            std::cout << "Invalid command" << std::endl;
        }
    }
}