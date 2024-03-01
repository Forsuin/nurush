#define FMT_HEADER_ONLY

#include <iostream>
#include <fstream>
#include <sstream>

#include <fmt/core.h>

#include "rufs.h"

bool expected_args(std::vector<std::string> &tokens, int expected)
{
    return tokens.size() == expected;
}

int main(int argc, char **argv)
{

    if (argc != 2)
    {
        fmt::println("Usage: rufs [filename]");
        return 1;
    }

    // root directory created automatically
    Filesystem fs(std::string(argv[1]) + ".bin");

    std::cout << "Welcome to RUFS. Enter one of the following commands: " << std::endl;
    std::cout << "CreateDir, CreateFile, EndDir, or quit" << std::endl;

    bool close = false;

    /*
        pwd - print absolute path of working directory
        ls - list names of all files and directories in working directory, including the type
             Also list name of directory
        cd <dirname> - change to specified directory
        cd .. - move to parent directory, ignore if root
        mkdir <name> - make a new directory with specified name
        cat <filename> - print contents of a text file or ignore invalid file type
        createFile <filename> - create a file with name and prompt user for contents
        printInfo - print out filesystem info from project 1
        quit - stop rush and return to linux shell
    */

    // Main REPL loop
    std::string input;
    while (!close)
    {
        std::cout << "Command> ";
        std::getline(std::cin, input, '\n');

        std::stringstream ss(input);
        std::string word;
        std::vector<std::string> tokens;

        while (ss >> word)
        {
            tokens.push_back(word);
        }

        if (tokens.size() > 2)
        {
            fmt::println("Invalid command: Too many arguments");
            continue;
        }

#define EX_ARGS(correct_token_count, expected__arg_count)                                                                  \
    do                                                                                                                     \
    {                                                                                                                      \
        if (!expected_args(tokens, correct_token_count))                                                                   \
        {                                                                                                                  \
            fmt::println("Invalid number of arguments. Expected {}, recieved {}", expected__arg_count, tokens.size() - 1); \
            continue;                                                                                                      \
        }                                                                                                                  \
    } while (0)

        if (tokens[0] == "pwd")
        {
            EX_ARGS(1, 0);
            fmt::println(fs.pwd());
        }
        else if (tokens[0] == "ls")
        {
            EX_ARGS(1, 0);
            fmt::println(fs.ls());
        }
        else if (tokens[0] == "cd")
        {
            EX_ARGS(2, 1);

            if (tokens[1] == "..")
            {
                fs.dotdot();
            }
            else
            {
                if (!fs.contains(tokens[1]))
                {
                    fmt::println("Directory {} does not exist", tokens[1]);
                    continue;
                }

                fs.change_dir(tokens[1]);
            }
        }
        else if (tokens[0] == "mkdir")
        {
            EX_ARGS(2, 1);

            if (tokens[1].length() > 8)
            {
                fmt::println("Invalid directory name, can only be 8 characters name long");
                continue;
            }

            fs.create_dir(tokens[1]);
        }
        else if (tokens[0] == "cat")
        {
            EX_ARGS(2, 1);
            fmt::println(fs.cat(tokens[1]));
        }
        else if (tokens[0] == "createFile")
        {
            EX_ARGS(2, 1);

            if (tokens[1].length() > 10)
            {
                fmt::println("Invalid file name, can be a maximum of 10 charactesr long");
                continue;
            }
            else if (tokens[1][tokens[1].length() - 2] != '.')
            {
                fmt::println("Missing file extension");
                continue;
            }
            else if (tokens[1].back() != 't' && tokens[1].back() != 'p')
            {
                fmt::println("Invalid file extension. Must be .p or .t");
                continue;
            }

            std::shared_ptr<Filable> file;

            if (input.back() == 't')
            {
                Text t;
                t.get_input();

                file = std::make_shared<Text>(t);
                file->name[9] = 't';
            }
            else
            {
                Program p;
                p.get_input();

                file = std::make_shared<Program>(p);
                file->name[9] = 'p';
            }

            // Get filename
            for (int i = 0; i < 8; i++)
            {
                file->name[i] = tokens[1][i];
            }
            file->name[8] = '.';
            file->name[10] = '\0';

            fs.create_file(file);
        }
        else if (tokens[0] == "printInfo")
        {
            EX_ARGS(1, 0);
            fmt::println(fs.get_info());
        }
        else if (tokens[0] == "quit")
        {
            EX_ARGS(1, 0);
            close = true;
            continue;
        }
    }
}