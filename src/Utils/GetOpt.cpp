//
// Created by miscelacreativa on 17/08/23.
//

#include "GetOpt.h"
#include <boost/program_options.hpp>
#include <iostream>
#include "../LOG/easylogging++.h"
namespace po = boost::program_options;

GetOpt::GetOpt(int ac, char **av) {
    try {

        po::options_description desc("Allowed options");
        desc.add_options()
                ("help,h", "produce help message")
                ("config,c",po::value<std::string>(),"config file")
                ("inst,i",po::value<std::string>(),"instance file")
                ;

        po::variables_map vm;
        po::store(po::parse_command_line(ac, av, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            std::exit(EXIT_SUCCESS);
        }

        if (vm.count("config")) {
            m_config = vm["config"].as<std::string>();
        }
        if (vm.count("inst")) {
            m_instance = vm["inst"].as<std::string>();
        }
        else{
            LOG(ERROR) << "instance file required.";
            std::exit(EXIT_FAILURE);
        }
    }
    catch(std::exception& e) {
        LOG(ERROR) << e.what();
        std::exit(EXIT_FAILURE);
    }
    catch(...) {
        LOG(ERROR) << "Exception of unknown type!";
        std::exit(EXIT_FAILURE);
    }
}
