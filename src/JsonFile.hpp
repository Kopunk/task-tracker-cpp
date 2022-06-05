#include <fstream>
#include <ctime>

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

    std::string avg_time_hrs(uint);

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
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::ostringstream stream;
    stream << std::put_time(&tm, "%F");
    std::string str = stream.str();
    return str;
}

std::string JsonFile::get_current_time()
{
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::ostringstream stream;
    stream << std::put_time(&tm, "%R");
    std::string str = stream.str();
    return str;
}

// get averged time in hours (in 0.25h intervals)
std::string JsonFile::avg_time_hrs(uint minutes)
{
    uint full_hrs = minutes / 60;
    uint remainder_minutes = minutes % 60;
    std::string avg_minutes;
    switch (remainder_minutes)
    {
    case 0 ... 5:
        avg_minutes = "00";
        break;
    case 6 ... 20:
        avg_minutes = "25";
        break;
    case 21 ... 35:
        avg_minutes = "50";
        break;
    case 36 ... 50:
        avg_minutes = "75";
        break;
    default:
        avg_minutes = "00";
        full_hrs++;
    }
    return std::to_string(full_hrs) + "." + avg_minutes;
}
// placed in this class to be inherited and to avoid duplication
