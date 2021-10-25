#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <sstream>
#include <string>
#include <map>

#define MAX_BUFFER_SIZE 64
//server
int main()
{
    //1
    std::string name_pipe;
    std::cout << "Please, enter the name of pipe: ";
    std::cin >> name_pipe;
    std::string full_name = "\\\\.\\pipe\\" + name_pipe;
    HANDLE pipe_y = CreateNamedPipe(full_name.c_str(),
                                    PIPE_ACCESS_DUPLEX,
                                    PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE,
                                    PIPE_UNLIMITED_INSTANCES,
                                    MAX_BUFFER_SIZE,
                                    MAX_BUFFER_SIZE,
                                    0,
                                    nullptr);
  if (pipe_y == INVALID_HANDLE_VALUE)
    {
        std::cout << "Error:"<< GetLastError()<<'\n';
    }

    //2 and 3
    std::string command (MAX_BUFFER_SIZE, '\0');
    std::string keyword,key,value,response {};
    DWORD bytes;
    std::map<std::string, std::string> data {};
    while (true)
    {
        std::cout << "Please , wait until the client connects...\n";
        if (!ConnectNamedPipe(pipe_y, nullptr))
        {
           std::cout << "Error:"<< GetLastError()<<'\n';;
           CloseHandle(pipe_y);
        }
        else
        {
           std::cout << "connection succeeded"<<'\n';;
        }
       while(true)
       {

            std::cout << "Please,wait for the client's com...\n";
            auto fr = ReadFile(pipe_y, &command[0], command.size(), &bytes, nullptr);
            if (!fr)
            {
                auto err = GetLastError();
                std::cout<<"Error:"<<err;
            }
            command.resize(command.find('\0'));

            std::istringstream parser {command};
            parser >> std::ws >> keyword;
            if (keyword == "set")
                {
                    parser >> key >> value;
                    data[key] = value;
                    response = "acknowledged";
                }
            else if (keyword == "get")
                {
                    parser >> key;
                    if (data.find(key) != data.end())
                        response = "found " + data[key];
                    else
                        response = "missing";
                }
            else if (keyword == "list")
                {
                    for (auto i = data.begin(); i != data.end(); ++ i)
                        response += i->first + " ";
                }
            else if (keyword == "delete")
                {
                    parser >> key;
                    auto del = data.find(key);
                    if (del != data.end())
                    {
                        data.erase(del);
                        response = "deleted";
                    }
                    else
                        response = "missing";
                }
            else if (keyword == "quit")
            {
                DisconnectNamedPipe(pipe_y);
                command.replace(0, command.size(), command.size(), '\0');
                command.resize(MAX_BUFFER_SIZE, '\0');
                break;
            }
             WriteFile(pipe_y, response.c_str(), response.size(), &bytes, nullptr);

             command.replace(0, command.size(), command.size(), '\0');
             command.resize(MAX_BUFFER_SIZE, '\0');
             response.clear();
             keyword.clear();
             key.clear();
             value.clear();
    }

         //5
         char answer;
         bool pt = true;
         std::cout << "Do you want to kill the pipe?(d,n)\n";
         while (std::cin>>answer)
         {
            if (answer == 'd')
            {
                CloseHandle(pipe_y);
                pt = false;
                break;
            }
            else if (answer == 'n')
                break;
            else
            {
                std::cout << "(y/n): ";
                continue;
            }
        }
        if (!pt)
            break;


    }

}