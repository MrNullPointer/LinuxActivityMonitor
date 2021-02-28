#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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

string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line{""};
  string label{""};
  string memTotal{"0"};
  string memFree{"0"};
  string buffers{"0"};
  float adjMem{1.0};  // adjusted memory
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.find("MemTotal:") != string::npos) {
        std::istringstream linestream(line);
        linestream >> label >> memTotal;
      } else if (line.find("MemFree:") != std::string::npos) {
        std::istringstream linestream(line);
        linestream >> label >> memFree;
      } else if (line.find("Buffers:") != std::string::npos) {
        std::istringstream linestream(line);
        linestream >> label >> buffers;
      }
    }
  }
  filestream.close();
  adjMem = stof(memTotal) - stof(buffers);
  return 1.0 - (stof(memFree) / adjMem);
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line{""};
  string uptime{""};
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  filestream.close();
  return stol(uptime);
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return UpTime() * sysconf(_SC_CLK_TCK); }

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line{""};
  long jiffies{0};
  std::ifstream filestream(kProcDirectory + to_string(pid));
  if (filestream.is_open()) {
    std::getline(filestream, line);
    // split line into a vector
    std::istringstream linestream(line);
    std::istream_iterator<string> start(linestream);
    std::istream_iterator<string> end;
    std::vector<string> stov(start, end);
    jiffies = stol(stov[13]) + stol(stov[14]) + stol(stov[15]) + stol(stov[16]);
  }
  filestream.close();
  return jiffies;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  string line{""};
  string num{"0"};
  long jiffies{0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    line.erase(line.begin(), line.begin() + 5);
    std::istringstream linestream(line);
    while (linestream >> num) {
      jiffies += std::stol(num);
    }
  }
  filestream.close();
  return jiffies;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  string line{""};
  long jiffies{0};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    line.erase(line.begin(), line.begin() + 5);
    // split line into a vector
    std::istringstream linestream(line);
    std::istream_iterator<string> start(linestream);
    std::istream_iterator<string> end;
    std::vector<string> cpu(start, end);
    jiffies = std::stol(cpu[kIdle_]) + std::stol(cpu[kIOwait_]);
  }
  filestream.close();
  return jiffies;
}

// Read and return CPU utilization

vector<string> LinuxParser::CpuUtilization() {
  string line{""};
  string num{"0"};
  vector<string> cpu;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    line.erase(line.begin(), line.begin() + 5);
    // split line into a vector
    std::istringstream linestream(line);
    while (linestream >> num) {
      cpu.push_back(num);
    }
  }
  filestream.close();
  return cpu;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line{""};
  string key{""};
  string value{"0"};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.find("processes") != string::npos) {
        std::istringstream linestream(line);
        linestream >> key >> value;
      }
    }
  }
  filestream.close();
  return stoi(value);
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line{""};
  string key{""};
  string value{"0"};
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.find("procs_running") != string::npos) {
        std::istringstream linestream(line);
        linestream >> key >> value;
      }
    }
  }
  filestream.close();
  return stoi(value);
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string cmd{""};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, cmd);
  }
  filestream.close();
  return cmd;
}

// Return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line{""};
  string vmSize{"0"};  // in kilobytes
  string label{""};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.find("VmSize:") != string::npos) {
        std::istringstream linestream(line);
        linestream >> label >> vmSize;
      }
    }
  }
  filestream.close();
  return to_string(std::stoi(vmSize) / 1024);  // in megabytes
}

// Read and return the user ID associated with a process

string LinuxParser::Uid(int pid) {
  string line{""};
  string label{""};
  string uid{""};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.find("Uid:") != string::npos) {
        std::istringstream linestream(line);
        linestream >> label >> uid;
      }
    }
  }
  filestream.close();
  return uid;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string ptrn{string(":x:").append(Uid(pid))};
  string line{""};
  string user{""};
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.find(ptrn) != string::npos) {
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream linestream(line);
        linestream >> user;
      }
    }
  }
  filestream.close();
  return user;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line{""};
  long uptime{1};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    // split line into a vector
    std::istringstream linestream(line);
    std::istream_iterator<string> start(linestream);
    std::istream_iterator<string> end;
    std::vector<string> stat(start, end);
    uptime = std::stol(stat[21]) / sysconf(_SC_CLK_TCK);  // seconds
  }
  filestream.close();
  return uptime;
}

float LinuxParser::CpuUtilization(int pid) {
  string line{""};
  string num{"0"};
  vector<string> stat{};
  float cpu{1.0};
  float totalTime{1.0};
  float seconds{1.0};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    // split line into a vector
    std::istringstream linestream(line);
    while (linestream >> num) {
      stat.push_back(num);
    }
  }
  filestream.close();
  totalTime =
      (stof(stat[13]) + stof(stat[14]) + stof(stat[15]) + stof(stat[16])) /
      sysconf(_SC_CLK_TCK);
  seconds = UpTime() - (stof(stat[21]) / sysconf(_SC_CLK_TCK));
  cpu = totalTime / seconds;
  return cpu;
}
