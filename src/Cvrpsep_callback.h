//
// Created by miscelacreativa on 21/08/23.
//

#ifndef TESTBC_CVRPSEP_CALLBACK_H
#define TESTBC_CVRPSEP_CALLBACK_H

#include <cstring>
#include <ilcplex/ilocplex.h>
#include <vector>
#include "Instance.h"
#include "CVRPSEP/cnstrmgr.h"

class CVRPSEP_CALLBACKI : public IloCplex::LazyConstraintCallbackI {
    IloEnv env;
public:
    struct EdgeStruct{
        int n_edges;
        const std::vector<int> edgeTail;
        const std::vector<int> edgeHead;
        const std::vector<double> edgeX;
    };
public:
    CVRPSEP_CALLBACKI(const IloEnv& env, IloArray<IloNumVarArray> & xi, Instance & inst):IloCplex::LazyConstraintCallbackI{env}, env{env}, m_xi{xi}, m_inst{inst}{init();};

    IloCplex::CallbackI* duplicateCallback() const;

    void main();
private:
    void init();
    void applyCut();
    void addConstraints(const std::vector<int>& S);
    EdgeStruct getEdges();
private:
    CnstrMgrPointer m_oldCutsCMP;
    Instance m_inst;
    IloArray<IloNumVarArray> m_xi;
    int m_nCut=0;
};

inline IloCplex::Callback CVRPSEP_CALLBACKI_handle(const IloEnv& env, IloArray<IloNumVarArray> & xi, Instance & inst) {
    return (IloCplex::Callback(new(env) CVRPSEP_CALLBACKI(env, xi, inst)));
}


#endif //TESTBC_CVRPSEP_CALLBACK_H
