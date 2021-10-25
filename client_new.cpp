
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <sstream>
#include <string>

#define MAX_BUFFER_SIZE 64

int main()//client
{   //1
    std::string name_pipe;
    std::cout<<"Please, enter the name of pipe:";
    std::cin >> name_pipe;
    std::string full_name = "\\\\.\\pipe\\" + name_pipe;

    HANDLE pipe_y = CreateFile(full_name.c_str(),
                               GENERIC_READ | GENERIC_WRITE,
                               0,
                               nullptr,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               nullptr);

    if (pipe_y == INVALID_HANDLE_VALUE)
    {
        std::cout << "Error:"<< GetLastError()<<std::endl;
    }

    //2
    std::string command (64,'\0');

    DWORD w;
    DWORD r;
    //5
    while (true)
    {
        std::cout << "> ";
        std::getline(std::cin >> std::ws, command);
        WriteFile(pipe_y, command.c_str(), command.size(), &w, nullptr);
        if (command == "quit") //3
        {
          CloseHandle(pipe_y);
          break;
        }
        else
        {
            std::string buf (MAX_BUFFER_SIZE, '\0');
            ReadFile(pipe_y, &buf[0], buf.size(), &r, nullptr);
            std::cout << buf <<'\n';
        }
        command.replace(0,command.size(),command.size(),'\0');
    }
}