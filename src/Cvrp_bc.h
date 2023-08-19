//
// Created by miscelacreativa on 17/08/23.
//

#ifndef TESTBC_CVRP_BC_H
#define TESTBC_CVRP_BC_H
#include "Instance.h"
#include "ConfigParams.h"

class Cvrp_bc {
public:
    Cvrp_bc(const Instance* instance,const ConfigParams* configParams );
private:
    const Instance * m_inst;
    const ConfigParams * m_configParams;
};


#endif //TESTBC_CVRP_BC_H
