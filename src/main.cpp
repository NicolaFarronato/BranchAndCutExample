#include <fmt/format.h>
#include "LOG/easylogging++.h"
#include "ConfigParams.h"
#include "InstanceCarp.h"
#include "Instance.h"
#include "Utils/GetOpt.h"
#include "Cvrp_bc.h"
#include "Cvrp_cplex_interface.h"
#include "InstanceWriter.h"
INITIALIZE_EASYLOGGINGPP

using namespace fmt;
int main(int argc, char **argv) {
    // Load configuration from file
    el::Configurations conf("easylogger_conf.conf");
    // Init loggers
    el::Loggers::getLogger("main");
    el::Loggers::getLogger("instance");
    el::Loggers::getLogger("instanceCARP");
    el::Loggers::getLogger("config");
    el::Loggers::getLogger("Cvrp_bc");
    el::Loggers::getLogger("optimizer");
    el::Loggers::getLogger("callback");
    el::Loggers::getLogger("InstanceWriter");
    el::Loggers::reconfigureAllLoggers(conf);
    LOG(INFO) << "Logger Initialized";



    // Parse arguments
    GetOpt opt {argc,argv};
    CLOG(INFO,"main") << "Arguments Parsed";

    InstanceCarp ic {opt.getInstance()};
    auto is = ic.convertToCVRP();
//
    ConfigParams cp {opt.getConfig()};
//     Parse instance file
//    Instance is {opt.getInstance()};
    if (!is.isValid())
        exit(EXIT_FAILURE);
    CLOG(INFO,"main") << "Instance initialized";

    InstanceWriter::WriteCvrp(is,"prova","/home/miscelacreativa/Desktop/Tesi/CODICE/INSTANCES/converted.vrp");

    Cvrp_cplex_interface cci {is,cp};
//    cci.solveModel();
//    cci.writeSolution();
    return 0;
}
