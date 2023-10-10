//
// Created by miscelacreativa on 10/10/23.
//

#ifndef TESTBC_INSTANCEWRITER_H
#define TESTBC_INSTANCEWRITER_H
#include "Instance.h"

class InstanceWriter {
public:
    static void WriteCvrp(Instance & is,std::string instanceName, const std::string& path);
};


#endif //TESTBC_INSTANCEWRITER_H
