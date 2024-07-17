#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include "log.h"

//setting default level to that so it doesn't get stuck with garbage memory
//but let's be honest, no one wants the INFO level logs being thrown into terminal
//that would be absurd levels of spam
int Log::curLevel = Log::INFO;
std::ofstream LogFile;

void Log::initalize(logLevel currentLevel){
    std::cout << "Initializing Logging System" << std::endl;
    LogFile.open("log.txt", std::ios_base::app);
    LogFile.close();
    Log::curLevel = currentLevel;
}

void Log::log(logLevel level, const char* var){
    std::time_t now = std::time(nullptr);
    std::tm timeinfo;
    localtime_s(&timeinfo, &now); // Use localtime_s for safer handling
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeinfo);

    std::string strLev = "";
    if(level == Log::INFO){
        strLev += "[INFO] ";
    }else if(level == Log::WARNING){
        strLev += "[WARNING] ";
    }else if(level == Log::ERROR){
        strLev += "[ERROR] ";
    }
    if(level >= Log::curLevel){
        std::cout << timestamp << " " << strLev << var << std::endl;
    }
    LogFile.open("log.txt", std::ios_base::app);
    LogFile << timestamp << " " << strLev << var << std::endl;
    LogFile.close();
}

void Log::setLevel(logLevel level){
    Log::curLevel = level;
}