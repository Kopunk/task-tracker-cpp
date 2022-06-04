#include <fstream>
#include <chrono>
#include <date.h>

#ifndef INCLUDE_NLOHMANN_JSON_HPP_
#include <json.hpp>
#endif

#ifndef LOG_H
#include <Log.h>
#endif

#define JSON_FILE_H

class JsonFile
{
    using json = nlohmann::json;

protected:
    const u_short JSON_INDENT = 2;
    std::string file_path;
    json file_json;

    void load_file();
    void save_file();

public:
    static std::string get_current_date();
    static std::string get_current_time();

    JsonFile(std::string file_path) : file_path(file_path){};
    ~JsonFile(){};
};

void JsonFile::load_file() // throws exception if file not found
{
    std::ifstream file(file_path);
    file >> file_json;
    plog::log("loaded json file:" + file_json.dump());
}

void JsonFile::save_file()
{
    std::ofstream file(file_path);
    file << std::setw(JSON_INDENT) << file_json << std::endl;
    plog::log("saved json file: " + file_path);
}

std::string JsonFile::get_current_date()
{
    return date::format("%F", std::chrono::system_clock::now());
}

std::string JsonFile::get_current_time()
{
    return date::format("%R", std::chrono::system_clock::now());
}
