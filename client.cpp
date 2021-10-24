#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <sstream>
#include <string>
#define MAX_BUFFER_SIZE 64
//клиент
int main()
{   //п1
    std::string pipe_name;
    std::cout<<"Enter pipe name :";
    std::cin >> pipe_name;
    std::string pipe = "\\\\.\\pipe\\" + pipe_name;
    HANDLE c_pipe = CreateFile(pipe.c_str(),
                               GENERIC_READ | GENERIC_WRITE,
                               0,
                               nullptr,
                               OPEN_EXISTING,
                               FILE_ATTRIBUTE_NORMAL,
                               nullptr);
    if (c_pipe == INVALID_HANDLE_VALUE)
    {
        std::cout << "Error:"<< GetLastError()<<std::endl;
    }

    //п2
    std::string command (64,'\0');
    DWORD w;
    DWORD r;
    //п5
    while (true)
    {
        std::cout << "> ";
        std::getline(std::cin >> std::ws, command);
        WriteFile(c_pipe, command.c_str(), command.size(), &w, nullptr);
        //п3
        if (command == "quit")
        {
          CloseHandle(c_pipe);
          break;
        }
        else
        {
            std::string buf (MAX_BUFFER_SIZE, '\0');
            ReadFile(c_pipe, &buf[0], buf.size(), &r, nullptr);
            std::cout << buf <<'\n';
        }
        command.replace(0,command.size(),command.size(),'\0');
    }
}
