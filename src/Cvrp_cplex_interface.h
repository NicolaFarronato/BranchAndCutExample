//
// Created by miscelacreativa on 17/08/23.
//

#ifndef TESTBC_CVRP_CPLEX_INTERFACE_H
#define TESTBC_CVRP_CPLEX_INTERFACE_H
#include <ilcplex/ilocplex.h>
#include "Instance.h"
#include "ConfigParams.h"
#include "Cvrpsep_callback.h"
#include "CVRP_SEP_GCB.h"
class Cvrp_cplex_interface {
public:
    Cvrp_cplex_interface(Instance & instance, ConfigParams & params);
    ~Cvrp_cplex_interface();
    void solveModel();
    void writeSolution();
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
    CVRP_SEP_GCB * m_cb;
    IloArray<IloNumVarArray> m_xi;

    void setParams();
};


#endif //TESTBC_CVRP_CPLEX_INTERFACE_H
