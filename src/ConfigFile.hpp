#include <vector>

#ifndef JSON_FILE_H
#include "JsonFile.hpp"
#endif

#ifndef LOG_H
#include <Log.h>
#endif

#define CONFIG_FILE_H

class ConfigFile : JsonFile
{
    using json = nlohmann::json;

private:
    struct schema
    {
        std::vector<std::string> project_names;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(schema, project_names)
    } schema;

public:
    ConfigFile(const std::string config_file_path) : JsonFile(config_file_path){};
    void load_config();
    void save_config();
    bool add_project(std::string);
    std::vector<std::string> get_projects();

    void print();
};

void ConfigFile::load_config()
{
    load_file();
    plog::log("load config file at " + this->file_path + ":\n" + file_json.dump(JSON_INDENT));
    schema = file_json;
}

// returns false if project name already exists
bool ConfigFile::add_project(std::string name)
{
    for (std::string it : schema.project_names)
    {
        if (it == name)
            return false;
    }
    schema.project_names.emplace_back(name);
    return true;
}

std::vector<std::string> ConfigFile::get_projects()
{
    return schema.project_names;
}

void ConfigFile::save_config()
{
    file_json = schema;
    plog::log("saving config file at " + this->file_path + ":\n" + file_json.dump(JSON_INDENT));
    save_file();
}

void ConfigFile::print(){};
