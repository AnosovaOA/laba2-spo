#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <sstream>
#include <string>
#include <map>
#define MAX_BUFFER_SIZE 64
//сервер
int main()
{
    //п1
    std::string pipe_name;
    std::cout << "Enter  pipe name: ";
    std::cin >> pipe_name;
    std::string pipe = "\\\\.\\pipe\\" + pipe_name;
    HANDLE c_pipe = CreateNamedPipe(pipe.c_str(),
                                    PIPE_ACCESS_DUPLEX,
                                    PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE,
                                    PIPE_UNLIMITED_INSTANCES,
                                    64,64,0,nullptr);
  if (c_pipe == INVALID_HANDLE_VALUE)
    {
        std::cout << "Error:"<< GetLastError()<<'\n';
    }
//п2,3
    std::string command (MAX_BUFFER_SIZE, '\0');
    std::string keyword,key,value,response {};
    DWORD bytes;
    std::map<std::string, std::string> data {};
    while (true)
    {
        std::cout << "Wait until the client connects...\n";
        if (!ConnectNamedPipe(c_pipe, nullptr))
        {
           std::cout << "Error:"<< GetLastError()<<'\n';;
           CloseHandle(c_pipe);
        }
        else
        {
           std::cout << "connection successful"<<'\n';;
        }
       while(true)
       {

            std::cout << "Wait for the client's com...\n";
            auto fr = ReadFile(c_pipe, &command[0], command.size(), &bytes, nullptr);
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
                DisconnectNamedPipe(c_pipe);
                command.replace(0, command.size(), command.size(), '\0');
                command.resize(MAX_BUFFER_SIZE, '\0');
                break;
            }
             WriteFile(c_pipe, response.c_str(), response.size(), &bytes, nullptr);

             command.replace(0, command.size(), command.size(), '\0');
             command.resize(MAX_BUFFER_SIZE, '\0');
             response.clear();
             keyword.clear();
             key.clear();
             value.clear();
    }


// п 5
        char answer;
        bool exit = false;
        std::cout << "Do you want to destroy pipe \"" << pipe_name << "\" (y/n)?: ";
        while (std::cin >> answer)
        {
            if (answer == 'y')
            {
                (CloseHandle(c_pipe),
                                  "occurred while closing pipe",
                                  pipe_name);
                exit = true;
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
        if (exit)
            break;
    }
}
