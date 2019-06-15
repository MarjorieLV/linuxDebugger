#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ptrace.h>

#include "debugger.hpp"

int main(int argc, char* argv[])
{
  if (argc < 2) {
    std::cout << "Please pass program name in arguments" << std::endl;
    return -1;
  }

  std::string program(argv[1]);

  pid_t processId = fork();

  if (processId == 0) {
    //execute program here
    std::cout << "child process" << std::endl;

    ptrace(PTRACE_TRACEME, 0, nullptr, nullptr);
    execl(program.c_str(), program.c_str(), nullptr);
  }
  else {
    //execute debugger here
    std::cout << "parent process " << processId << std::endl;

    Debugger debugger(program, processId);
    debugger.run();
  }
  return 0;
}
