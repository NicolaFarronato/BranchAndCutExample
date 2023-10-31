//
// Created by miscelacreativa on 17/08/23.
//

#ifndef TESTBC_INSTANCE_H
#define TESTBC_INSTANCE_H


#include <string>
#include <vector>

class Instance {
public:
    Instance(const std::string& fn);
    Instance(int nVertices,int capacity, int minNumVehicles, std::vector<std::vector<double>> dij,
             std::vector<double> d,std::vector<std::pair<double,double>> coord, bool isCarp= false,
             std::vector<std::vector<int>> spaths = std::vector<std::vector<int>>(),int origCarpVertices=0);
    int getVertices() {return  m_nvertices;}
    int getCapacity() {return m_capacity;}
    int getMinNumVehicles() {return m_minNumVehicles;}
    std::vector<std::vector<double>> getCosts() {return m_dij;}
    std::vector<double> getDemand() {return m_d;}
    std::vector<std::pair<double,double>> getCoord() {return m_coord;}
    bool isValid() {return checkIsValid();}
    bool isCARP() {return m_isCarp;}
    std::vector<std::vector<int>> getSP() {return m_shortestPaths;}
    int getCarpVertices(){return m_Vcarp;}
private:
    std::vector<std::pair<double,double>> readNodesCoordinates(std::ifstream& file) const;
    std::vector<std::vector<double>> readWeights(std::ifstream& file) const;
    std::vector<double> readDemands(std::ifstream& file) const;
    bool checkIsValid();
private:
    bool m_isCarp;
    int m_nvertices,m_capacity, m_minNumVehicles,m_Vcarp;
    std::vector<std::vector<double>> m_dij;
    std::vector<double> m_d;
    std::vector<std::pair<double,double>> m_coord;
    std::vector<std::vector<int>> m_shortestPaths;
};





#endif //TESTBC_INSTANCE_H
