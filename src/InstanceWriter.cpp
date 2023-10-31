//
// Created by miscelacreativa on 10/10/23.
//

#include <fstream>
#include "InstanceWriter.h"
#include "fmt/format.h"
#include "LOG/easylogging++.h"

void InstanceWriter::WriteCvrp(Instance &is,std::string instanceName, const std::string& path) {


    std::ofstream outTxt (path);
    if (!outTxt.is_open()){
        CLOG(INFO,"InstanceWriter") << "Unable to open file";
        return;
    }

    // name
    outTxt << fmt::format("NAME : {}\n",instanceName);
    //comment
    outTxt << fmt::format("COMMENT : Generated from CARP\n");
    outTxt << fmt::format("TYPE : ECVRP\n");
    outTxt << fmt::format("DIMENSION : {}\n",is.getVertices());
    outTxt << fmt::format("EDGE_WEIGHT_TYPE : EXPLICIT\n");
    outTxt << fmt::format("EDGE_WEIGHT_FORMAT : LOWER_ROW\n");
    outTxt << fmt::format("DISPLAY_DATA_TYPE : NO_DISPLAY\n");
    outTxt << fmt::format("CAPACITY : {}\n",is.getCapacity());
    outTxt << fmt::format("EDGE_WEIGHT_SECTION\n");

    int nbRows = std::ceil(((std::pow(is.getVertices(), 2) - is.getVertices()) / 2)/10);
    int all_entries = (int)(std::pow(is.getVertices(), 2) - is.getVertices()) / 2;
    int i{1},j{0};
    for (int c = 0; c < nbRows; ++c) {
        int rem_entries = all_entries-10 >= 0 ? 10 : all_entries;
        std::vector<double> entries;
        for (int k = 0; k < rem_entries; ++k) {
            if (j >= i)
            {
                j = 0;
                ++i;
            }
            entries.push_back(is.getCosts()[i][j]);
            ++j;
        }
        all_entries-=10;
        outTxt << fmt::format("{}\n",fmt::join(entries, "     "));
    }

    outTxt << fmt::format("DEMAND_SECTION\n");

    for (int k = 0; k < is.getVertices(); ++k) {
        outTxt << fmt::format("{} {}\n", k + 1, is.getDemand()[k]);
    }

    outTxt << fmt::format("N_NODES\n");
    outTxt << fmt::format("{}\n",is.getCarpVertices());
    outTxt << fmt::format("SP_SECTION\n");

    for (const auto & sp : is.getSP()) {
        outTxt << fmt::format("{} .\n",fmt::join(sp, "  "));
    }

    outTxt << fmt::format("DEPOT_SECTION\n");
    outTxt << fmt::format("1\n");
    outTxt << fmt::format("-1\n");
    outTxt << fmt::format("EOF\n");

    outTxt.close();
}
