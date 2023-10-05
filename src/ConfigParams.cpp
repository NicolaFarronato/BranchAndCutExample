//
// Created by miscelacreativa on 16/08/23.
//

#include "ConfigParams.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
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

    // Open the file
    std::ifstream file(fn);

    // Read the entire file into a string
    std::string json((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());
    Document d;
    d.Parse(json.c_str());
    int error_code;
    if (d.HasParseError())
    {
        CLOG(ERROR,"config") << fmt::format("File {} has parse error {}.",fn,d.GetParseError());
        std::exit(EXIT_FAILURE);
    }
    if ( d.HasMember("timeLimit") )
        timeLimit = d["timeLimit"].GetInt();
    if ( d.HasMember("outputDir") )
    {
        outputDir = d["outputDir"].GetString();
        try{
            std::filesystem::create_directory(outputDir);
        }
        catch(...){}
    }


}
