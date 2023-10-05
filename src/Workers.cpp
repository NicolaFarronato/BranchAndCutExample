//
// Created by miscelacreativa on 04/10/23.
//

#include "Workers.h"
#include "CVRPSEP/capsep.h"
Workers::Workers(const std::vector<double> & demandVec, int nCostumer, double capacity):
m_demand(demandVec),m_nCostumer(nCostumer),m_capacity(capacity) {
    CMGR_CreateCMgr(&m_cutsCMP,100);
    CMGR_CreateCMgr(&m_oldCutsCMP,100);
    m_nVertices= nCostumer+1;
}

std::vector<std::pair<std::vector<int>,int>> Workers::findCut(IloArray<IloNumArray> & curr_xi) {

    std::vector<std::pair<std::vector<int>,int>> ret;
    auto currEdgeStruct = getEdges(curr_xi);
    if (currEdgeStruct.n_edges < 1) return ret;
    char IntegerAndFeasible;
    double MaxViolation;


    auto EdgeTail = currEdgeStruct.edgeTail.data();
    auto EdgeHead = currEdgeStruct.edgeHead.data();
    auto EdgeX = currEdgeStruct.edgeX.data();

    CAPSEP_SeparateCapCuts(m_nCostumer,
                           m_demand.data(),
                           m_capacity,
                           currEdgeStruct.n_edges,
                           EdgeTail,
                           EdgeHead,
                           EdgeX,
                           m_oldCutsCMP,
                           8,
                           1e-6,
                           1e-6,
                           &IntegerAndFeasible,
                           &MaxViolation,
                           m_cutsCMP);
    if (IntegerAndFeasible) return ret; /* Optimal solution found */
    if (m_cutsCMP->Size == 0) return ret; /* No cuts found */

    int i,j;
    double RHS;

    for (int c = 0; c < m_cutsCMP->Size; ++c)
    {
        if (m_cutsCMP->CPL[c]->CType == CMGR_CT_CAP)
        {
            std::vector<int> list;
            list.reserve(m_cutsCMP->CPL[c]->IntListSize);
            for (j = 1; j <= m_cutsCMP->CPL[c]->IntListSize; ++j)
            {
                auto val = m_cutsCMP->CPL[c]->IntList[j];
                list.push_back(val == m_nVertices +1 ? 0 : val);
            }


            /* Now List contains the customer numbers defining the cut. */
            /* The right-hand side of the cut, */
            /* in the form x(S:S) <= |S| - k(S), is RHS. */
            RHS = m_cutsCMP->CPL[c]->RHS;


            /* Add the cut to the LP. */
            std::pair<std::vector<int>,int> currRes {list,RHS};
            ret.push_back(currRes);
        }
    }
    for (i=0; i<m_cutsCMP->Size; i++)
    {
        CMGR_MoveCnstr(m_cutsCMP,m_oldCutsCMP,i,0);
    }
    m_cutsCMP->Size = 0;
    return ret;
}

Workers::~Workers() {
    CMGR_FreeMemCMgr(&m_cutsCMP);
    CMGR_FreeMemCMgr(&m_oldCutsCMP);
}

Workers::EdgeStruct Workers::getEdges(IloArray<IloNumArray> & curr_xi) {
    int n_edges = 0;

    for (int i = 0; i < m_nVertices; ++i) {
        for (int j = i+1; j < m_nVertices; ++j) {
            if (curr_xi[i][j] > 1e-6)
                ++n_edges;
        }
    }

    std::vector<int> edgeTail;
    std::vector<int> edgeHead;
    std::vector<double> edgeX;

    if (n_edges > 0) {
        edgeTail.reserve(n_edges + 1);
        edgeHead.reserve(n_edges + 1);
        edgeX.reserve(n_edges + 1);
        /* skip first element */
        edgeTail.push_back(0);
        edgeHead.push_back(0);
        edgeX.push_back(0);

        for (int i = 0; i < m_nVertices; ++i)
        {
            for (int j = i + 1; j < m_nVertices; ++j)
            {
                auto val = curr_xi[i][j];
                if (val > 1e-6)
                {
                    edgeTail.push_back(i == 0 ? m_nVertices : i);
                    edgeHead.push_back(j);
                    edgeX.push_back(val);
                }
            }
        }

    }


    return Workers::EdgeStruct({
                                    n_edges,edgeTail,edgeHead,edgeX
                            });
}
