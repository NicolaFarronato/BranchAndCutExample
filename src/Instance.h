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
    int getVertices() {return  m_nvertices;}
    int getCapacity() {return m_capacity;}
    int getMinNumVehicles() {return m_minNumVehicles;}
    std::vector<std::vector<double>> getCosts() {return m_dij;}
    std::vector<double> getDemand() {return m_d;}
    bool isValid() {return checkIsValid();}
private:
    std::vector<std::pair<double,double>> readNodesCoordinates(std::ifstream& file) const;
    std::vector<std::vector<double>> readWeights(std::ifstream& file) const;
    std::vector<double> readDemands(std::ifstream& file) const;
    bool checkIsValid();
private:
    int m_nvertices,m_capacity, m_minNumVehicles;
    std::vector<std::vector<double>> m_dij;
    std::vector<double> m_d;
};


#endif //TESTBC_INSTANCE_H
