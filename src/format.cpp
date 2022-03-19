#include <string>
#include <iomanip>
#include "format.h"
#include <iostream>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int minutes = 0;
    std::stringstream stream;
    
    seconds = seconds % 3600;
    minutes = seconds / 60;
    seconds = seconds % 60; 

    stream << std::setfill('0') << std::setw(2) << ":" << minutes << ":" << std::setw(2) << seconds;
    return stream.str();


} 