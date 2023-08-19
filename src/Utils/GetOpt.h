//
// Created by miscelacreativa on 17/08/23.
//



#ifndef TESTBC_GETOPT_H
#define TESTBC_GETOPT_H


#include <string>

class GetOpt {
public:
    GetOpt(int argc, char **argv);
    std::string GetConfig() {return m_config;};
    std::string GetInstance() {return m_instance;};
private:
    std::string m_config, m_instance;
};


#endif //TESTBC_GETOPT_H
