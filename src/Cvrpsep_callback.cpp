//
// Created by miscelacreativa on 21/08/23.
//
#include <cstring>
#include <fmt/format.h>
#include "Cvrpsep_callback.h"
#include "LOG/easylogging++.h"
#include "CVRPSEP/capsep.h"




double EpsForIntegrality = 0.0001;


IloCplex::CallbackI* CVRPSEP_CALLBACKI::duplicateCallback() const {
    return (new(getEnv()) CVRPSEP_CALLBACKI(*this));
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
    int MaxNoOfCuts = 8;



    auto currEdgeStruct = getEdges();

    NoOfCustomers = m_inst.getVertices()-1;


    const double * Demand= m_inst.getDemand().data();

    CAP = (double)m_inst.getCapacity();

    NoOfEdges = currEdgeStruct.n_edges;
    EdgeTail = currEdgeStruct.edgeTail.data();
    EdgeHead = currEdgeStruct.edgeHead.data();
    EdgeX = currEdgeStruct.edgeX.data();

    CnstrMgrPointer cutsCMP;
    CMGR_CreateCMgr(&cutsCMP,100);
    /* Contains no cuts initially */
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
                           m_oldCutsCMP,
                           MaxNoOfCuts,
                           EpsForIntegrality,
                           EpsForIntegrality,
                           &IntegerAndFeasible,
                           &MaxViolation,
                           cutsCMP);
    if (IntegerAndFeasible) return; /* Optimal solution found */
    if (cutsCMP->Size == 0) return; /* No cuts found */

    int i,j;
    double RHS;

    for (int c = 0; c < cutsCMP->Size; ++c)
    {
        if (cutsCMP->CPL[c]->CType == CMGR_CT_CAP)
        {
            std::vector<int> list;
            list.reserve(cutsCMP->CPL[c]->IntListSize);
            for (j = 1; j <= cutsCMP->CPL[c]->IntListSize; ++j)
            {
                auto val = cutsCMP->CPL[c]->IntList[j];
                list.push_back(val == NoOfCustomers +1 ? 0 : val);
            }


            /* Now List contains the customer numbers defining the cut. */
            /* The right-hand side of the cut, */
            /* in the form x(S:S) <= |S| - k(S), is RHS. */
//            RHS = cutsCMP->CPL[i]->RHS;


            /* Add the cut to the LP. */

            addConstraints(list);
        }
    }
/* Read the cuts from MyCutsCMP, and add them to the LP */
/* Resolve the LP */
/* Move the new cuts to the list of old cuts: */
//    for (i=0; i<cutsCMP->Size; i++)
//    {
//        CMGR_MoveCnstr(cutsCMP,m_oldCutsCMP,i,0);
//    }
//    cutsCMP->Size = 0;
//    } while (true);


    CMGR_FreeMemCMgr(&cutsCMP);
    ++m_nCut;
    }

CVRPSEP_CALLBACKI::EdgeStruct CVRPSEP_CALLBACKI::getEdges() {

//    These three vectors give information on the current
//    LP solution. Only information on those edges e with x∗e > 0
//    should be passed in the three vectors. EdgeTail[e], EdgeHead[e]
//    are the two end vertices of edge e. EdgeX[e] is x∗e .
//            The depot is assumed to be numbered n + 1. Note that edge numbers
//    are 1-based (e = 1, . . . , NoOfEdges).

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
                auto val = getValue(m_xi[i][j]);
                if (val > EpsForIntegrality)
                {
                    edgeTail.push_back(i == 0 ? n_edges : i);
                    edgeHead.push_back(j);
                    edgeX.push_back(val);
                }
            }
        }

    }


    return CVRPSEP_CALLBACKI::EdgeStruct({
        n_edges,edgeTail,edgeHead,edgeX
    });
}

void CVRPSEP_CALLBACKI::addConstraints(const std::vector<int>& S) {
    std::string name = fmt::format("S_{}",m_nCut);
    double qS = 0;
    int n = m_inst.getVertices();


    for (int currNodeDemand : S)
        qS += m_inst.getDemand()[currNodeDemand];

    IloNum roundedCap = ceil(qS/m_inst.getCapacity());

    std::vector<int> notS;
    for (int i = 0; i < n; ++i) {
        if(std::find(S.begin(), S.end(), i) == S.end())
            notS.push_back(i);
        else continue;
    }



    IloExpr expr(getEnv());
    expr.clear();

    for (auto i:S) {
        for (auto j:notS) {
            if (i >= j) continue;

            expr += m_xi[i][j];
        }
    }
    for (auto i:notS) {
        for(auto j:S){
            if (i >= j) continue;

            expr += m_xi[i][j];
        }
    }


//    for (int i = 0; i < n; ++i) {
//        if(std::find(S.begin(), S.end(), i) != S.end()) continue;
//        for (int j = 0; j < n; ++j) {
//            if(std::find(S.begin(), S.end(), j) != S.end() || i >= j) continue;
//            expr += m_xi[i][j];
//        }
//    }
//
//    for (int i = 0; i < n; ++i) {
//        for (int j = 0; j < n; ++j) {
//            if(std::find(S.begin(), S.end(), j) != S.end()) continue;
//            if ( i >= j) continue;
//            expr += m_xi[i][j];
//        }
//    }

    IloRange capConstraint {env,2*roundedCap,expr,IloInfinity,name.c_str()};
    add(capConstraint).end();


    expr.clear();
    expr.end();
}

void CVRPSEP_CALLBACKI::init() {
    CMGR_CreateCMgr(&m_oldCutsCMP,100);
}

