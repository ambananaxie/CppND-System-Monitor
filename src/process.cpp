#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;


// Return this process's ID
int Process::Pid() { 
    return pid_; 
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
    std::vector<int> list = LinuxParser::CpuUtilization(Pid());
    if(!list.empty()){
        int uptime = LinuxParser::UpTime();
        int total = list[0] + list[1];
        int starttime = list[4];
        float hertz = sysconf(_SC_CLK_TCK);
        int secondsPassed = uptime - starttime / hertz;
        
        if(secondsPassed > 0){
            cpuUtilization = total / hertz / secondsPassed;
        }
      
    }
    return cpuUtilization;
}

// TODO: Return the command that generated this process
string Process::Command() { 
    string command = LinuxParser::Command(pid_);
    string cmd = "";
    if(command.size() > 40){
        for(int i = 0; i <= 40; i++){
            cmd = cmd + command[i];
        }
        cmd = cmd + "...";
        return cmd;
    }
    return command; 
}

// TODO: Return this process's memory utilization
string Process::Ram() { 
    return LinuxParser::Ram(pid_); 
}

// TODO: Return the user (name) that generated this process
string Process::User() { 
    return LinuxParser::User(pid_); 
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { 
    return LinuxParser::UpTime() - LinuxParser::UpTime(pid_); 
}

// TODO: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
    return a.cpuUtilization < this->cpuUtilization; 
}