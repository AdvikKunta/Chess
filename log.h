#pragma once

class Log{

public:

    enum logLevel{
        INFO,
        WARNING,
        ERROR
    };

    static int curLevel;

    static void initalize(logLevel);

    static void log(logLevel level, const char* var);

    static void setLevel(logLevel level);


};