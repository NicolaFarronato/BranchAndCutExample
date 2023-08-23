//
// Created by miscelacreativa on 16/08/23.
//

#ifndef TESTBC_CONFIGPARAMS_H
#define TESTBC_CONFIGPARAMS_H


#include <string>

class ConfigParams {
public:
    ConfigParams();
    ConfigParams(std::string fn);
public:
    int timeLimit {3600};
    std::string outputDir;
private:

};


#endif //TESTBC_CONFIGPARAMS_H
