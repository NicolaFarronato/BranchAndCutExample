//
// Created by miscelacreativa on 21/08/23.
//
#include <cstring>
#include "Cvrpsep_callback.h"
#include "LOG/easylogging++.h"
#include "CVRPSEP/capsep.h"
#include "CVRPSEP/cnstrmgr.h"



double EpsForIntegrality = 0.0001;


IloCplex::CallbackI* CVRPSEP_CALLBACKI::duplicateCallback() const {
    return (new(getEnv()) CVRPSEP_CALLBACKI(*this));
}


std::vector<std::pair<int, int>> CVRPSEP_CALLBACKI::getXiValues() {



    return std::vector<std::pair<int, int>>();
}

void CVRPSEP_CALLBACKI::main() {


    CLOG(INFO,"callback") << "Candidate cb.";
    applyCut();

}

void CVRPSEP_CALLBACKI::applyCut() {
    char IntegerAndFeasible;
    int NoOfCustomers,NoOfEdges;
    double MaxViolation, CAP;
    const int   *EdgeTail, *EdgeHead;
    const double *EdgeX;
    int MaxNoOfCuts = 20;



    auto currEdgeStruct = getEdges();

    NoOfCustomers = m_inst.getVertices()-1;


    const double * Demand= m_inst.getDemand().data();

    CAP = (double)m_inst.getCapacity();

    NoOfEdges = currEdgeStruct.n_edges;
    EdgeTail = currEdgeStruct.edgeTail.data();
    EdgeHead = currEdgeStruct.edgeHead.data();
    EdgeX = currEdgeStruct.edgeX.data();

    CnstrMgrPointer MyCutsCMP,MyOldCutsCMP;
    CMGR_CreateCMgr(&MyCutsCMP,100);
    CMGR_CreateCMgr(&MyOldCutsCMP,100); /* Contains no cuts initially */
/* Allocate memory for the three vectors EdgeTail, EdgeHead, and EdgeX */
/* Solve the initial LP */

//    do
//    {
/* Store the information on the current LP solution */
/* in EdgeTail, EdgeHead, EdgeX. */
/* Call separation. Pass the previously found cuts in MyOldCutsCMP: */
        CAPSEP_SeparateCapCuts(NoOfCustomers,
                               Demand,
                               CAP,
                               NoOfEdges,
                               EdgeTail,
                               EdgeHead,
                               EdgeX,
                               MyOldCutsCMP,
                               MaxNoOfCuts,
                               EpsForIntegrality,
                               EpsForIntegrality,
                               &IntegerAndFeasible,
                               &MaxViolation,
                               MyCutsCMP);
//        if (IntegerAndFeasible) break; /* Optimal solution found */
//        if (MyCutsCMP->Size == 0) break; /* No cuts found */

        int i,j,ListSize;
        double RHS;
        std::vector<int> list;

        for (i=0; i<MyCutsCMP->Size; i++)
        {
            ListSize=0;
            for (j=1; j<=MyCutsCMP->CPL[i]->IntListSize; j++)
            {
                auto val = (MyCutsCMP->CPL[i]->IntList[j]);
                list.push_back( val == NoOfCustomers+1 ? 0 : val);
            }

            /* Now List contains the customer numbers defining the cut. */
            /* The right-hand side of the cut, */
            /* in the form x(S:S) <= |S| - k(S), is RHS. */
            RHS = MyCutsCMP->CPL[i]->RHS;
            /* Add the cut to the LP. */

            LOG(INFO) << "speta";


        }



/* Read the cuts from MyCutsCMP, and add them to the LP */
/* Resolve the LP */
/* Move the new cuts to the list of old cuts: */
//        for (int i=0; i<MyCutsCMP->Size; i++)
//        {
//            CMGR_MoveCnstr(MyCutsCMP,MyOldCutsCMP,i,0);
//        }
//        MyCutsCMP->Size = 0;
//    } while (true);


    CMGR_FreeMemCMgr(&MyCutsCMP);
    CMGR_FreeMemCMgr(&MyOldCutsCMP);
}

CVRPSEP_CALLBACKI::EdgeStruct CVRPSEP_CALLBACKI::getEdges() {



    int n_edges = 0;
    int nVertices = m_inst.getVertices();
    for (int i = 0; i < nVertices; ++i) {
        for (int j = i+1; j < nVertices; ++j) {
            if (getValue(m_xi[i][j]) > EpsForIntegrality)
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

        for (int i = 0; i < nVertices; ++i)
        {
            for (int j = i + 1; j < nVertices; ++j)
            {
                if (getValue(m_xi[i][j]) > EpsForIntegrality)
                {
                    edgeTail.push_back(i == 0 ? n_edges : i);
                    edgeHead.push_back(j);
                    edgeX.push_back(getValue(m_xi[i][j]));
                }
            }
        }

    }


    return CVRPSEP_CALLBACKI::EdgeStruct({
        n_edges,edgeTail,edgeHead,edgeX
    });
}

CVRPSEP_CALLBACKI::~CVRPSEP_CALLBACKI() = default;
