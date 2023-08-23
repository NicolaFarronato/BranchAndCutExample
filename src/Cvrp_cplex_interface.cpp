//
// Created by miscelacreativa on 17/08/23.
//

#include "Cvrp_cplex_interface.h"
#include "LOG/easylogging++.h"
#include <fmt/format.h>

Cvrp_cplex_interface::Cvrp_cplex_interface(Instance & instance, ConfigParams & params) :
m_instance(instance),
m_params(params),
m_model(m_env),
m_cplex(m_env)
{
    initModel();
}


void Cvrp_cplex_interface::solveModel() {
    if (m_params.outputDir != "")
//        m_cplex.exportModel(fmt::format("{}/model.lp",m_params.outputDir).c_str());
    try
    {
        if ( !m_cplex.solve() ) {
            m_env.error() << "Failed to optimize LP" << std::endl;
            throw (-1);
        }

        std::cout << "\n\nCplex success!\n";
        std::cout << "\tStatus: " << m_cplex.getStatus() << "\n";
        std::cout << "\tObjective value: " << m_cplex.getObjValue() << "\n";
        auto n = m_instance.getVertices();
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                std::cout << fmt::format("value for x{}{} ",i,j) << m_cplex.getValue(m_xi[i][j]) << "\n";
            }
        }
    }
    catch (IloException& e) {
        LOG(ERROR) << "Fallita ottimizzazione";
//        cerr << "Concert exception caught: " << e << endl;
    }
    catch (...) {
        LOG(ERROR) << "Fallita ottimizzazione";
//        cerr << "Unknown exception caught" << endl;
    }
}





void Cvrp_cplex_interface::initModel() {
    /// Two Index Vehicle Flow Formulation
    /* G = (V,E)
     * V = {0, \dots,n}
     * V' = {1, \dots, n}
     * Vertex 0 : depot
     * n : costumers
     * Cost d_{ij} \meq 0 , i,j \in E
     * m number of identical vehicles
     * Q capacity of each vehicle
     *
     * r(S) minimum number of vehicles of capacity Q needed to satisfy the demand of costumers in S \in $
     * q(S) = sum_{i \in S} q_i : total demand of node subset S
     * \xi_{ij} integer variable with values {0,1} or {0,1,2} means arc ij selected by the route
     *
     * Formulation
     * min \sum_{(i,j)\in E} d_{i,j} \xi_{i,j}
     * subject to :
     *  (2) sum_{j \in V i<j}xi_{ij} + sum_{j \in V i>j}xi_{ji} = 2 \forall i in V' degree constraint
     *  (3) GSEC sum_{i in S}sum_{j in \bar{S} i<j} xi_{ij} + sum_{i in \bar{S}}sum_{j in S i<j} xi_{ij} >= 2r(S) forall S in $
     *  (4)   \sum_{j \in V'} \xi_{0j} = 2m m vechicles must leave and return to the depot
     *  (5-6) \xi_{ij} integer variable with values {0,1} or {0,1,2}
     * */

    // initialize xi
    IloExpr expr(m_env);
    auto xi = initXi();
    m_xi = xi;
    setDegreeConstraint(xi,expr);
    setDepotReturnConstraint(xi,expr);
    setObjectiveFunction(xi,expr);

    expr.clear();
    expr.end();

    setParams();

    m_cplex= IloCplex(m_model);
//    IloCplex prova(m_model);
//    m_cplex.exportModel(fmt::format("{}/model.lp",m_params.outputDir).c_str());
    m_cplex.exportModel("model.lp");

}

IloArray<IloNumVarArray> Cvrp_cplex_interface::initXi() {

    int n = (int)m_instance.getCosts().size();
    IloArray<IloNumVarArray> xi(m_env, n);
    for(int i = 0; i < n; ++i) {
        xi[i] = IloNumVarArray(m_env, n);
        for(int j = 0; j < n; ++j) {
            std::string name = fmt::format("xi_{}_{}",i,j);
//            if (j == 0)
//            {
//                xi[i][j] = IloNumVar(m_env,name.c_str());
//                continue;
//            }
            xi[i][j] = IloNumVar(m_env, 0,
                                 (i == 0 && j > 0) ? 2:1,
                                 (i == 0 && j > 0) ? IloNumVar::Int : IloNumVar::Bool,
                                 name.c_str());
        }
    }
    return xi;
}

void Cvrp_cplex_interface::setObjectiveFunction(IloArray<IloNumVarArray> & xi, IloExpr & expr) {
    expr.clear();

    auto n_vertices = m_instance.getVertices();
    for (int i = 0; i < n_vertices; ++i) {
        for (int j = 0; j < n_vertices; ++j) {
            expr += xi[i][j] * m_instance.getCosts()[i][j];
        }
    }

    IloObjective obj(m_env, expr, IloObjective::Minimize);
    m_model.add(obj);

}

void Cvrp_cplex_interface::setDepotReturnConstraint(IloArray<IloNumVarArray> & xi, IloExpr & expr) {
    expr.clear();

//    std::string name_m {"m"};
    std::string name_con = "C4";
    const IloNum minNumVehicles =m_instance.getMinNumVehicles(); // fixed number of vehicles
//    IloNumVar m {m_env,
//                 minNumVehicles,
//                 2*minNumVehicles, // todo capire cosa fare qua
//                 IloNumVar::Int,
//                 name_m.c_str()};
    auto n_vertices = m_instance.getVertices();
    const int i = 0;
    for (int j = 1; j < n_vertices; ++j) {
        expr += xi[i][j];
    }
    IloRange depotRetCon {m_env,2*minNumVehicles,expr,2*minNumVehicles,name_con.c_str()};
    m_model.add(depotRetCon);
}

void Cvrp_cplex_interface::setDegreeConstraint(IloArray<IloNumVarArray> &xi, IloExpr &expr) {
    expr.clear();

    std::string cname;
    int n = (int)m_instance.getCosts().size();
    IloRangeArray degCon(m_env, n);
    for (int i = 1; i < n; ++i) {

        cname = fmt::format("C2_{}",i);
        expr.clear();
        for (int j = i-1; j >= 0 ; --j)
            expr += xi[j][i];
        for (int j = i+1; j <n ; ++j)
            expr += xi[i][j];
        degCon[i] = IloRange (m_env,2,expr,2,cname.c_str());
    }
    m_model.add(degCon);
}

void Cvrp_cplex_interface::setParams() {
    m_cplex.setParam(IloCplex::Param::TimeLimit, m_params.timeLimit);

}

