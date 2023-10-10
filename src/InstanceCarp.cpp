//
// Created by miscelacreativa on 09/10/23.
//

#include "InstanceCarp.h"
#include <fstream>
#include <numeric>
#include <filesystem>
#include <utility>
#include "LOG/easylogging++.h"
#include <fmt/format.h>


InstanceCarp::InstanceCarp(const std::string &fn) {
    if(! std::filesystem::exists(fn))
    {
        CLOG(ERROR,"instanceCARP") << fmt::format("File {} does not exists.",fn);
        std::exit(EXIT_FAILURE);
    }
    std::ifstream file{fn};
    std::string line;
    std::string edgeWeightType;
    while (std::getline(file,line))
    {
        std::istringstream iss(line);
        std::string ckey,tmp,cval;
        iss >> ckey;
        iss >> tmp >> cval;
        bool print = true;
        if (ckey == "VERTICES")
        {
            m_nvertices = std::stoi(cval);
            m_cij.resize(m_nvertices,std::vector<double>(m_nvertices, 0));
        }
        else if (ckey == "CAPACIDAD")
            m_capacity = std::stoi(cval);
        else if (ckey == "ARISTAS_REQ")
            m_arcReq = std::stoi(cval);
        else if (ckey == "ARISTAS_NOREQ")
            m_arcNoReq = std::stoi(cval);
        else if (ckey == "DEPOSITO")
            m_depot = std::stoi(cval);
        else if (ckey == "VEHICULOS")
            m_nVehicles = std::stoi(cval);
        else if (ckey == "LISTA_ARISTAS_REQ"){
            print = false;
            readReq(file, true);
        }
        else if (ckey == "LISTA_ARISTAS_NOREQ"){
            print = false;
            readReq(file, false);
        }
        else if( (ckey == "COMENTARIO") || (ckey == "NOMBRE") || (ckey == "VEHICULOS") || (ckey == "COSTE_TOTAL_REQ")){
            while (iss >> tmp)
                cval+=(" " +tmp);
        }
        else print = false;
        CLOG_IF(print,INFO,"instanceCARP") << fmt::format("{} : {}",ckey,cval);
    }
}

void InstanceCarp::readReq(std::ifstream &file, bool isReq){

    int nRows = isReq ? m_arcReq : m_arcNoReq;
    for (int i = 0; i < nRows; ++i) {

        std::string line;
        std::getline(file, line);
        std::istringstream iss(line);
        std::string tmp;
        double c,d{0.0};
        int x,y;
        if (isReq)
        {
            iss >> tmp >> x >> tmp >> y >> tmp >> tmp >> c >> tmp >> d;
            m_dij[{x,y}]=d;
            m_edges.emplace_back(x,y);
        }
        else{
            iss >> tmp >> x >> tmp >> y >> tmp >> tmp >> c;
            m_edgesNoReq.emplace_back(x,y);
        }
        --x;
        --y;
        m_cij[x][y] = c;
        m_cij[y][x] = c;
    }
}





Instance InstanceCarp::convertToCVRP() {

    int nVerticesCvrp = 2*(int)m_edges.size()+1;
    m_es = fwShortestPaths();
    std::vector<std::vector<double>> dij (nVerticesCvrp,std::vector<double>(nVerticesCvrp));
    std::vector<double> demands (nVerticesCvrp,0);
    int it_r{1},it_c{1},it_z{1};
    for (const auto &[key_ij,v_ij] : m_dij)
    {
        for (const auto &[key_kl,v_kl] : m_dij)
        {
            dij[it_r][it_c]       = (key_ij == key_kl) ?
                                  0 :    0.5 * m_cij[key_ij.first-1][key_ij.second-1]
                                         + getPathCost(key_ij.first,key_kl.first)
                                         + 0.5* m_cij[key_kl.first-1][key_kl.second-1];
            dij[it_r+1][it_c] = (key_ij == key_kl) ?
                                    0 :  0.5 * m_cij[key_ij.first-1][key_ij.second-1]
                                         + getPathCost(key_ij.second,key_kl.first)
                                         + 0.5* m_cij[key_kl.first-1][key_kl.second-1];
            dij[it_r][it_c+1] = (key_ij == key_kl) ?
                                    0 :    0.5 * m_cij[key_ij.first-1][key_ij.second-1]
                                           + getPathCost(key_ij.first,key_kl.second)
                                           + 0.5* m_cij[key_kl.first-1][key_kl.second-1];
            dij[it_r+1][it_c+1]   = (key_ij == key_kl) ?
                                    0 :    0.5 * m_cij[key_ij.first-1][key_ij.second-1]
                                           + getPathCost(key_ij.second,key_kl.second)
                                           + 0.5* m_cij[key_kl.first-1][key_kl.second-1];

            dij[it_c][it_r] = dij[it_r][it_c];
            dij[it_c][it_r+1] = dij[it_r+1][it_c];
            dij[it_c+1][it_r] = dij[it_r][it_c+1];
            dij[it_c+1][it_r+1] = dij[it_r+1][it_c+1];
            it_c+=2;
        }
        // depot zero //
        dij[0][it_z] = getPathCost(m_depot,key_ij.first) + 0.5*m_cij[key_ij.first-1][key_ij.second-1];
        dij[0][it_z+1] = getPathCost(m_depot,key_ij.second) + 0.5*m_cij[key_ij.first-1][key_ij.second-1];
        dij[it_z][0] = dij[0][it_z];
        dij[it_z+1][0] = dij[0][it_z+1];

        // nodes demands //
        demands[it_r] = 0.5*v_ij*10;
        demands[it_r+1] = 0.5*v_ij*10;

        it_z+=2;
        it_r+=2;
        it_c=1;
    }


    return Instance{nVerticesCvrp,m_capacity*10,m_nVehicles,
                    dij,demands,std::vector<std::pair<double,double>> {},true};
}

InstanceCarp::fwEdgesStruct InstanceCarp::fwShortestPaths() {

    int nCostumer = m_nvertices;
    std::map<std::pair<int,int>,double> dist;
    std::map<std::pair<int,int>,int> next;


    for (int i = 1; i < nCostumer; ++i) {
        for (int j = i+1; j <= nCostumer; ++j) {
            dist[{i,j}] = 1e9;
            dist[{j,i}] = 1e9;
            next[{i,j}] = -1;
        }
        dist[{i,i}] = 0.0;
        next[{i,i}] = i;
    }

    for (auto e : m_edges)
    {
        std::pair<int,int> er{e.second,e.first};
        dist[e]=m_cij[e.first-1][e.second-1];
        dist[er]=m_cij[e.first-1][e.second-1];
        next[e]=e.second;
        next[er]=e.first;
    }

    for (auto e : m_edgesNoReq)
    {
        std::pair<int,int> er{e.second,e.first};
        dist[e]=m_cij[e.first-1][e.second-1];
        dist[er]=m_cij[e.first-1][e.second-1];
        next[e]=e.second;
        next[er]=e.first;
    }


    for (int k = 1; k < nCostumer+1; ++k) {
        for (int i = 1; i < nCostumer+1; ++i) {
            for (int j = 1; j < nCostumer+1; ++j) {
                if ( dist[{i,j}] > dist[{i,k}]+dist[{k,j}])
                {
                    dist[{i,j}] = dist[{i,k}]+dist[{k,j}];
                    next[{i,j}] = next[{i,k}];
                }
            }
        }
    }

    return (fwEdgesStruct{dist,next});
}

double  InstanceCarp::getPathCost(int u, int v) {
    double pathCost{0.0};
    if ( u > v)
    {
        int tmp = v;
        v=u;
        u=tmp;
    }
    if (m_es.next[{u,v}] == -1)
        return pathCost;
    int prev = u;
    while (u!=v){
        u = m_es.next[{u,v}];
        pathCost += prev<u ? m_cij[prev-1][u-1] : m_cij[u-1][prev-1] ;
        prev = u;
    }
    return pathCost;
}
