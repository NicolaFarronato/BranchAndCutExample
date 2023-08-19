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
    bool IsValid() {return CheckIsValid();}
private:
    std::vector<std::pair<double,double>> ReadNodesCoordinates(std::ifstream& file) const;
    std::vector<std::vector<double>> ReadWeights(std::ifstream& file) const;
    std::vector<double> ReadDemands(std::ifstream& file) const;
    bool CheckIsValid();
private:
    int m_nvertices,m_capacity;
    std::vector<std::vector<double>> m_cij;
    std::vector<double> m_d;
};


#endif //TESTBC_INSTANCE_H
