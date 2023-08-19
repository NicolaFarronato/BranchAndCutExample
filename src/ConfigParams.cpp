//
// Created by miscelacreativa on 16/08/23.
//

#include "ConfigParams.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <filesystem>
#include <fmt/format.h>
#include "LOG/easylogging++.h"

using namespace rapidjson;

ConfigParams::ConfigParams() = default;

ConfigParams::ConfigParams(std::string fn) {
    if(! std::filesystem::exists(fn))
    {
        CLOG(ERROR,"config") << fmt::format("File {} does not exists.",fn);
        std::exit(EXIT_FAILURE);
    }
    Document d;
    d.Parse(fn.c_str());
}
