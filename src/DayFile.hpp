#include <vector>
#include <set>

#ifndef JSON_FILE_H
#include "JsonFile.hpp"
#endif

#ifndef LOG_H
#include <Log.h>
#endif

#ifndef CONFIG_FILE_H
#include "ConfigFile.hpp"
#endif

#define DAY_FILE_H

class DayFile : public JsonFile
{
    using json = nlohmann::json;

private:
    friend class StateFile;

    struct timeline_element
    {
        std::string name;
        uint minutes;
        std::string start_time;
        std::string end_time;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(timeline_element, name, minutes, start_time, end_time)
    } timeline_element;

    // struct timeline
    // {
    //     std::vector<struct timeline_element> timeline_array;
    //     NLOHMANN_DEFINE_TYPE_INTRUSIVE(timeline, timeline_array)
    // } timeline;

    struct schema
    {
        std::string date;
        std::vector<struct timeline_element> timeline;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(schema, date, timeline)
    } schema;

    std::string sum_day_task_avg_hrs(std::string);
    uint sum_day_task_minutes(std::string);

public:
    DayFile(const std::string day_files_path, std::string date) : JsonFile(day_files_path + "/" + date + ".json")
    {
        schema.date = date;
    };

    void load_day();
    void save_day();
    std::string get_stats();
};

void DayFile::load_day()
{
    try
    {
        load_file();
    }
    catch (const nlohmann::detail::parse_error &e)
    {
        plog::log("error while reading the file " + this->file_path);
        save_day();
        load_file();
    }

    plog::log("load day file at " + this->file_path + ":\n" + file_json.dump(JSON_INDENT));
    schema = file_json;
}

void DayFile::save_day()
{
    file_json = schema;
    plog::log("saving day file at " + this->file_path + ":\n" + file_json.dump(JSON_INDENT));
    save_file();
}

uint DayFile::sum_day_task_minutes(std::string name)
{
    uint min_sum = 0;
    for (auto it : schema.timeline)
    {
        if (it.name == name)
            min_sum += it.minutes;
    }
    return min_sum;
}

std::string DayFile::sum_day_task_avg_hrs(std::string name)
{
    return avg_time_hrs(sum_day_task_minutes(name));
}

// returns stats excluding running tasks (e.g. from other days)
std::string DayFile::get_stats()
{
    using namespace std;

    if (schema.timeline.empty())
    {
        return "No records from " + schema.date + "\n";
    }

    string ret_str = "";
    set<string> projects;
    for (auto it : schema.timeline)
    {
        projects.insert(it.name);
    }
    for (auto it : projects)
    {
        string task_name = it;
        string avg_time = sum_day_task_avg_hrs(task_name);
        string time_min = to_string(sum_day_task_minutes(task_name));
        task_name.append(32 - task_name.length(), ' ');
        ret_str += "\t- " + task_name + avg_time + "h" + "\t(" + time_min + " min)\n";
    }
    return ret_str;
}
