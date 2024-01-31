#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "datetime.h"
#include <string>
#include <ctime>

const std::string getnowtime(const int offset)
{
    auto now = std::chrono::system_clock::now()+ std::chrono::minutes(offset);
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time_t);
    const int TIME_BUF_SIZE = 80;
    char time_buffer[TIME_BUF_SIZE];
    std::strftime(time_buffer, TIME_BUF_SIZE, "%Y:%m:%d %H:%M:%S", now_tm);
    std::string timestamp(time_buffer);
    //std::cout << "Current time: " << timestamp << std::endl;
    return timestamp;
}

/*
int main() {
    
    std::string nowtime=getnowtime();

    std::cout<<nowtime<<std::endl;

    return 0;
}*/
