//
// Created by miscelacreativa on 04/10/23.
//

#include "CVRP_SEP_GCB.h"
#include "CVRPSEP/capsep.h"
#include "fmt/format.h"
#include "LOG/easylogging++.h"


void CVRP_SEP_GCB::invoke(const IloCplex::Callback::Context &context) {

    auto thNum = context.getIntInfo(IloCplex::Callback::Context::Info::ThreadId);
    if (context.inThreadUp()) {
        delete m_w[thNum];
        m_w[thNum] = new Workers(m_inst.getDemand(),m_inst.getVertices()-1,m_inst.getCapacity());
        return;
    }

    // teardown
    if (context.inThreadDown()) {
        delete m_w[thNum];
        m_w[thNum] = nullptr;
        return;
    }
    if ( context.inCandidate() )
//        LOG(DEBUG) <<  fmt::format("thread num : {}",thNum);
        lazyCapacity (context,m_w[thNum]);
}

void CVRP_SEP_GCB::lazyCapacity(const IloCplex::Callback::Context &context, Workers * worker) {

    int NoOfCustomers;
    double CAP;
    NoOfCustomers = m_inst.getVertices()-1;
    int n = (int)m_inst.getCosts().size();
    IloEnv env = context.getEnv();
    IloArray<IloNumArray> xi(env, n);
    for (int i = 0; i < n; ++i) {
        xi[i] = IloNumArray(env, n);
        for (int j = i+1; j < n; ++j) {
            xi[i][j] = context.getCandidatePoint(m_xi[i][j]);
        }
    }

    CAP = (double)m_inst.getCapacity();
    const auto demVec = m_inst.getDemand();

    std::vector<std::pair<std::vector<int>,int>> cuts;
    {
        const std::lock_guard<std::mutex> lock(m_mutex);
        cuts = worker->findCut(xi);
    }

    for (IloInt i = 0; i < n; ++i) xi[i].end();
    xi.end();

    if (cuts.empty()) return; /* Optimal solution found */

    for (int c = 0; c < cuts.size(); ++c)
    {
            addCut(context,cuts[c].first,cuts[c].second);
            ++m_nCut;

    }

}

void CVRP_SEP_GCB::addCut(const IloCplex::Callback::Context &context,std::vector<int> & list, double rhs) {
    std::string name = fmt::format("S_{}",m_nCut);
    IloNumExpr sum = IloExpr(context.getEnv());


    for(auto i:list)
        for(auto j:list)
            if (i<j)
                sum+=m_xi[i][j];
//    LOG(DEBUG) << "Adding lazy capacity constraint " << sum << " <= " << rhs << std::endl;
    context.rejectCandidate(sum <= (IloNum)rhs);
    sum.end();
}


CVRP_SEP_GCB::~CVRP_SEP_GCB() {
    IloInt numWorkers = m_w.size();
    for (IloInt w = 0; w < numWorkers; w++) {
        delete m_w[w];
    }
    m_w.clear();
}
