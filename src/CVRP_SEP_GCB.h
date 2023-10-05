//
// Created by miscelacreativa on 04/10/23.
//

#ifndef TESTBC_CVRP_SEP_GCB_H
#define TESTBC_CVRP_SEP_GCB_H

#include <ilcplex/ilocplexi.h>
#include "Instance.h"
#include "ConfigParams.h"
#include "CVRPSEP/cnstrmgr.h"
#include "Workers.h"
class CVRP_SEP_GCB : public IloCplex::Callback::Function{

public:
    CVRP_SEP_GCB(IloArray<IloNumVarArray> & xi,Instance & instance, ConfigParams & params, IloInt numWorkers=1) :
                m_xi(xi),m_inst(instance),m_configParams(params),m_w(numWorkers, nullptr){CMGR_CreateCMgr(&m_oldCutsCMP,100);}
    ~CVRP_SEP_GCB() override;
    void invoke(const IloCplex::Callback::Context& context) override;
private:
    void lazyCapacity (const IloCplex::Callback::Context &context, Workers * worker);
    void addCut(const IloCplex::Callback::Context &context,std::vector<int> & list,double rhs);
private:
    IloArray<IloNumVarArray>  m_xi;
    Instance m_inst;
    ConfigParams m_configParams;
    CnstrMgrPointer m_oldCutsCMP;
    int m_nCut=0;
    double EpsForIntegrality = 0.0001;
    std::vector<Workers*> m_w;
};


#endif //TESTBC_CVRP_SEP_GCB_H
