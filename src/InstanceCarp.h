//
// Created by miscelacreativa on 09/10/23.
//

#ifndef TESTBC_INSTANCECARP_H
#define TESTBC_INSTANCECARP_H


#include <string>
#include <vector>
#include <map>
#include "Instance.h"

class InstanceCarp {
public:
    InstanceCarp(const std::string& fn);
    Instance convertToCVRP();
private:
    struct fwEdgesStruct{
        std::map<std::pair<int,int>,double> dist;
        std::map<std::pair<int,int>,int> next;
    };
private:
    void readReq(std::ifstream& file, bool isReq);
    fwEdgesStruct fwShortestPaths();
    std::vector<int> getPath(int u, int v);
private:
    int m_nvertices,m_capacity,m_arcReq,m_arcNoReq;
    std::vector<std::vector<double>> m_cij,m_dij;
    std::vector<std::pair<int,int>> m_edges;
    fwEdgesStruct m_es;
};


#endif //TESTBC_INSTANCECARP_H
