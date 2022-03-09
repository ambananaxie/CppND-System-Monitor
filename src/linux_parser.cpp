#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

template <typename T>
T readfileWithKey(string const &key, string const &file){
  T value;
  string line;
  string k;

  std::ifstream stream(LinuxParser::kProcDirectory + file);
  if(stream.is_open()){
    while(std::getline(stream,line)){
      std::istringstream streamline(line);
      while(streamline >> k >> value){
        if(k == key){
          return value;
        }
      }
    }
  }
  return value;
};

template <typename T>
T readFile(string const &file){
  T value;
  string line;
  std::ifstream stream(LinuxParser::kProcDirectory + file);
  if(stream.is_open()){
    std::getline(stream,line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  return value;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
    string key;
    string value;
    float memTotal = readfileWithKey<float>(tarMemTotal, kMeminfoFilename);
    float memFree = readfileWithKey<float>(tarMemFree, kMeminfoFilename);
    return (memTotal - memFree)/memTotal;

}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  long uptime = readFile<long>(kUptimeFilename);
  return uptime;
 }

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return UpTime() * sysconf(_SC_CLK_TCK);
}

// TODO: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
    std::vector<int> list = CpuUtilization(pid);
    long active = list[0] + list[1] + list[2] + list[3];
    return active;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  std::vector<std::string> numbers = CpuUtilization(); 
  long active_jiffies = 0;
  for(string i : numbers){
    active_jiffies += std::stol(numbers[1]) + std::stol(numbers[2]) + std::stol(numbers[3]);
  }
  return active_jiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  std::vector<std::string> numbers = CpuUtilization();
  long idle_jiffies = std::stol(numbers[3]) + std::stol(numbers[4]);
  return idle_jiffies;  
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string firstline, num;
  vector<string> numbers{};
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, firstline);
    std::istringstream linestream(firstline);
    linestream >> num; // get numbers after "cpu"
    while (linestream >> num)
    {
      numbers.emplace_back(num);
    }
  }
  return numbers; 
  
}

vector<int> LinuxParser::CpuUtilization(int pid){
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);

  if (stream.is_open()) {
    int utime; //CPU time spent in user code, measured in clock ticks 
    int stime; //CPU time spent in kernel code, measured in clock ticks
    int cutime; //Waited-for children's CPU time spent in user code in clock ticks
    int cstime; //Waited-for children's CPU time spent in kernel code in clock ticks
    int starttime; //Time when the process started, measured in clock ticks

    std::string line, time;
    while (std::getline(stream, time)) { 
      std::istringstream linestream(line);
      for (int i = 1; i <= 22; i++) {
        linestream >> time;
      
        if (i == 14) {
          utime = stoi(time);    
        } 
        else if (i == 15) {
          stime = stoi(time);
        }
        else if (i == 16) {
          cutime = stoi(time);
        }
        else if (i == 17) {
          cstime = stoi(time);
        }
        else if (i == 22) {
          starttime = stoi(time);
        }
      }
      return {utime, stime, cutime, cstime, starttime};
    }
     
  }
  return {};
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  int proNum = readfileWithKey<int>(tarProcesses, kStatFilename);
  return proNum;
 }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  int proRunningNum = readfileWithKey<int>(tarProcessRunning, kStatFilename);
  return proRunningNum;
 }

// TODO: Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string processCommand;
  std::ifstream stream(kProcDirectory+std::to_string(pid)+kCmdlineFilename);
  if(stream.is_open()){
    std::getline(stream, processCommand);
  }
  return processCommand;
 }

// TODO: Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string filename = std::to_string(pid)+ kStatusFilename;
  int ram = readfileWithKey<int>(tarVmRSS, filename);
  ram /= 1000;
  return std::to_string(ram);
 }

// TODO: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string filename = std::to_string(pid)  + kStatusFilename;
  string uid = readfileWithKey<string>(tarUid, filename);
  return uid;
 }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string number;
  string name;
  string x;
  string line;
  string uid = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);
  if(stream.is_open()){
    while(std::getline(stream, line)){
      std::replace(line.begin(), line.end(),':',' ');
      std::istringstream linestream(line);
      linestream >> name >> x >> number;
      if(number == uid){
        return name;
      }
    }
  }
  return name;

 }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  long uptime;
  float hertz = sysconf(_SC_CLK_TCK);
  string line;
  int value, starttime;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(stream.is_open()){
    std::getline(stream, line);
    std::istringstream linestream(line);
    for(int i = 1; i <= 22; i++){
      linestream >> value;
      if(i == 22){
        starttime = value;
      }
    }
    uptime = starttime / hertz;
  }
  return uptime;
 }