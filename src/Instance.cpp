//
// Created by miscelacreativa on 17/08/23.
//

#include <fstream>
#include <filesystem>
#include "Instance.h"
#include "LOG/easylogging++.h"
#include <fmt/format.h>


std::vector<std::vector<double>> EuclidDist(
        const std::vector<std::pair<double, double>>& coord)
{
    std::vector<std::vector<double>> cij(coord.size(),
                                         std::vector<double>(coord.size(), 0));

    for (std::size_t i = 0; i < coord.size(); ++i)
    {
        for (std::size_t j = i + 1; j < coord.size(); ++j)
        {
            auto tmp = std::sqrt(std::pow(coord[i].first - coord[j].first, 2) +
                                 std::pow(coord[i].second - coord[j].second,2));
            cij[i][j] = (int)(std::round(tmp));
            cij[j][i] = cij[i][j];
        }
    }
    return cij;
}

Instance::Instance(const std::string& fn) {
    if(! std::filesystem::exists(fn))
    {
        CLOG(ERROR,"instance") << fmt::format("File {} does not exists.",fn);
        std::exit(EXIT_FAILURE);
    }
    std::ifstream file{fn};
    std::string line;
    std::string edgeWeightType;
    std::vector<std::pair<double, double>> coord;
    while (std::getline(file,line))
    {
        std::istringstream iss(line);
        std::string ckey,tmp,cval;
        iss >> ckey;
        iss >> tmp >> cval;
        bool print = true;
        if (ckey == "TYPE")
        {
            iss >> tmp >> cval;
            if (!(cval == "CVRP"))
            {
                CLOG(ERROR,"instance") << fmt::format("Instance {} is not CVRP.",ckey);
                std::exit(EXIT_FAILURE);
            }
        }
        else if (ckey == "DIMENSION")
            m_nvertices = std::stoi(cval);
        else if (ckey == "CAPACITY")
            m_capacity = std::stoi(cval);
        else if (ckey == "EDGE_WEIGHT_TYPE")
            edgeWeightType = cval;
        else if (ckey == "NODE_COORD_SECTION"){
            coord = ReadNodesCoordinates(file);
            print = false;
        }
        else if (ckey == "EDGE_WEIGHT_SECTION")
        {
            m_cij = ReadWeights(file);
            print = false;
        }
        else if (ckey == "DEMAND_SECTION")
        {
            m_d = ReadDemands(file);
            print = false;
        }
        else if( (ckey == "COMMENT") || (ckey == "NAME")){
            while (iss >> tmp)
                cval+=(" " +tmp);
        }
        else print = false;
        CLOG_IF(print,INFO,"instance") << fmt::format("{} : {}",ckey,cval);

    }
    if (edgeWeightType == "EUC_2D")
        m_cij = EuclidDist(coord);

}

std::vector<std::vector<double>> Instance::ReadWeights(std::ifstream &file) const {
    std::vector<std::vector<double>> cij(m_nvertices,
                                         std::vector<double>(m_nvertices, 0));

    int nbRows = std::ceil(((std::pow(m_nvertices, 2) - m_nvertices) / 2)/10);
    int c = 0, i = 1, j = 0;
    while (c < nbRows)
    {
        std::string line;
        std::getline(file, line);
        std::istringstream iss(line);
        double w;
        while (iss >> w)
        {
            if (j >= i)
            {
                j = 0;
                ++i;
            }
            cij[i][j] = w;
            cij[j][i] = w;
            ++j;
        }
        ++c;
    }

    return cij;
}

std::vector<std::pair<double,double>> Instance::ReadNodesCoordinates(std::ifstream &file) const {
    std::vector<std::pair<double, double>> coord;

    int i = 0;
    while (i < m_nvertices)
    {
        std::string line;
        std::getline(file, line);
        std::istringstream iss(line);
        double x, y;
        iss >> x >> x >> y; // skip index
        coord.emplace_back(x, y);
        ++i;
    }

    return coord;
}

std::vector<double> Instance::ReadDemands(std::ifstream &file) const {
    std::vector<double> demands;

    int i = 0;
    while (i < m_nvertices)
    {
        std::string line;
        std::getline(file, line);
        std::istringstream iss(line);
        double di;
        iss >> di >> di; // skip index
        demands.push_back(di);
        ++i;
    }

    return demands;
}

bool Instance::CheckIsValid() {
    bool ret = true;
    if (m_nvertices == 0)
    {
        CLOG(ERROR,"instance") << "Instance has zero vertices.";
        ret &=false;
    }
    if (m_capacity == 0)
    {
        CLOG(ERROR,"instance") << "Instance has zero vertices.";
        ret &=false;
    }
    if (m_d.empty())
    {
        CLOG(ERROR,"instance") << "Instance has empty demand.";
        ret &=false;
    }
    if (m_cij.empty())
    {
        CLOG(ERROR,"instance") << "Instance has empty costs.";
        ret &=false;
    }
    CLOG_IF(!ret,ERROR,"instance") << "Instance is not valid. Return.";
    return ret;
}

