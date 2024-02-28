//
// Created by miscelacreativa on 04/10/23.
//

#ifndef TESTBC_CVRP_SEP_GCB_H
#define TESTBC_CVRP_SEP_GCB_H

#include <ilcplex/ilocplexi.h>
#include <mutex>
#include "Instance.h"
#include "ConfigParams.h"
#include "CVRPSEP/cnstrmgr.h"
#include "Workers.h"
class Cvrp_sep_gcb : public IloCplex::Callback::Function{

public:
    Cvrp_sep_gcb(IloArray<IloNumVarArray> & xi,Instance & instance, ConfigParams & params, IloInt numWorkers=1) :
                m_xi(xi),m_inst(instance),m_configParams(params),m_w(numWorkers, nullptr){CMGR_CreateCMgr(&m_oldCutsCMP,100);}
    ~Cvrp_sep_gcb() override;
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
    std::mutex m_mutex;
};


#endif //TESTBC_CVRP_SEP_GCB_H
