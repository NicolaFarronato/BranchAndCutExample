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
            m_dij.resize(m_nvertices,std::vector<double>(m_nvertices, 0));
        }
        else if (ckey == "CAPACIDAD")
            m_capacity = std::stoi(cval);
        else if (ckey == "ARISTAS_REQ")
            m_arcReq = std::stoi(cval);
        else if (ckey == "ARISTAS_NOREQ")
            m_arcNoReq = std::stoi(cval);
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
        double c,d;
        int x,y;
        if (isReq)
            iss >> tmp >> x >> tmp >> y >> tmp >> tmp >> c >> tmp >> d;
        else
            iss >> tmp >> x >> tmp >> y >> tmp >> tmp >> c;
        m_edges.emplace_back(x,y);
        --x;
        --y;
        m_cij[x][y] = c;
        m_cij[x][y] = c;
        m_dij[x][y] = d;
        m_dij[y][x] = d;
    }
}

Instance InstanceCarp::convertToCVRP() {

    int nVerticesCvrp = 2*(int)m_edges.size()+1;
    m_es = fwShortestPaths();
    std::vector<std::vector<double>> dij (nVerticesCvrp,std::vector<double>(nVerticesCvrp));

    for (auto e_ij : m_es.dist)
    {
        for (auto e_kl : m_es.dist)
        {

        }

    }



    return Instance("");
}

InstanceCarp::fwEdgesStruct InstanceCarp::fwShortestPaths() {

    int nCostumer = m_nvertices+1;
    std::map<std::pair<int,int>,double> dist;
    std::map<std::pair<int,int>,int> next;


    for (int i = 0; i < nCostumer; ++i) {
        for (int j = i+1; j < nCostumer; ++j) {
            dist.insert(std::pair<std::pair<int,int>,double>(std::pair<int,int>(i,j), 1e9 ));
            dist.insert(std::pair<std::pair<int,int>,double>(std::pair<int,int>(j,i), 1e9 ));
            next.insert(std::pair<std::pair<int,int>,int>(std::pair<int,int>(i,i), -1 ));
        }
        dist.insert(std::pair<std::pair<int,int>,double>(std::pair<int,int>(i,i), 0.0 ));
        next.insert(std::pair<std::pair<int,int>,int>(std::pair<int,int>(i,i), i ));
    }

    for (auto e : m_edges)
    {
        std::pair<int,int> er{e.second,e.first};
        dist[e]=m_cij[e.first][e.second];
        dist[er]=m_cij[e.first][e.second];
        next[e]=e.second;
        next[er]=e.first;
    }

    for (int k = 0; k < nCostumer; ++k) {
        for (int i = 0; i < nCostumer; ++i) {
            for (int j = 0; j < nCostumer; ++j) {
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

std::vector<int> InstanceCarp::getPath(int u, int v) {
    std::vector<int> path;
    if (m_es.next[{u,v}] == -1)
        return path;
    path.push_back(u);
    while (u!=v){
        u = m_es.next[{u,v}];
        path.push_back(u);
    }
    return path;
}
