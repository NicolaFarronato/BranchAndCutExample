//
// Created by miscelacreativa on 17/08/23.
//

#include "Cvrp_bc.h"
#include <cplex.h>
#include "LOG/easylogging++.h"

Cvrp_bc::Cvrp_bc(const Instance *instance, const ConfigParams *configParams) :
    m_inst(instance),
    m_configParams(configParams)
{
    int error;
	CPXENVptr env = CPXopenCPLEX(&error);
    CLOG_IF(error,ERROR,"Cvrp_bc") << "CPXopenCPLEX() error";
	CPXLPptr lp = CPXcreateprob(env, &error, "CVRP bc");
    CLOG_IF(error,ERROR,"Cvrp_bc") << "CPXcreateprob() error";




}
