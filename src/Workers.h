//
// Created by miscelacreativa on 04/10/23.
//

#ifndef TESTBC_WORKERS_H
#define TESTBC_WORKERS_H

#include <vector>
#include <ilconcert/iloenv.h>
#include "CVRPSEP/cnstrmgr.h"


class Workers {
public:
    struct EdgeStruct{
        int n_edges;
        const std::vector<int> edgeTail;
        const std::vector<int> edgeHead;
        const std::vector<double> edgeX;
    };
public :
    Workers(const std::vector<double> & demandVec, int nCostumer, double capacity);
    ~Workers();
    std::vector<std::pair<std::vector<int>,int>> findCut(IloArray<IloNumArray> & curr_xi);
private:
    EdgeStruct getEdges(IloArray<IloNumArray> & curr_xi);
private :
    const std::vector<double> m_demand;
    int m_nCostumer;
    int m_nVertices;
    double m_capacity;
    CnstrMgrPointer m_oldCutsCMP;
    CnstrMgrPointer m_cutsCMP;
    std::mutex m_mutex;
};


#endif //TESTBC_WORKERS_H
