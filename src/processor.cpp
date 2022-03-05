#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    float activeJiffies;
    float idleJiffies;
    std::ifstream stream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
    if(stream.is_open()){
        std::getline(stream,line);
        std::istringstream linestream(line);
        linestream >> cpu >> user >> nice >> system >> idle >> irq >> sortirq >> steal;
    }

    activeJiffies = user + nice + system + irq + sortirq + steal;
    idleJiffies = idle + iowait;

    return activeJiffies/idleJiffies; 
}