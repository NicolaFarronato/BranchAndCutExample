//
// Created by miscelacreativa on 17/08/23.
//

#ifndef TESTBC_CVRP_CPLEX_INTERFACE_H
#define TESTBC_CVRP_CPLEX_INTERFACE_H
#include <ilcplex/ilocplex.h>
#include "Instance.h"
#include "ConfigParams.h"


class Cvrp_cplex_interface {
public:
    Cvrp_cplex_interface(Instance & instance, ConfigParams & params);
    void solveModel();
private:
    void initModel();
    IloArray<IloNumVarArray> initXi();
    void setObjectiveFunction(IloArray<IloNumVarArray> & xi, IloExpr & expr);
    void setDepotReturnConstraint(IloArray<IloNumVarArray> &xi, IloExpr &expr);
    void setDegreeConstraint(IloArray<IloNumVarArray> &xi, IloExpr &expr);
private:
    IloEnv m_env;
    IloModel m_model;
    IloCplex m_cplex;
    Instance m_instance;
    ConfigParams m_params;

    IloArray<IloNumVarArray> m_xi;

    void setParams();
};


#endif //TESTBC_CVRP_CPLEX_INTERFACE_H
