#include <vector>

#ifndef JSON_FILE_H
#include "JsonFile.hpp"
#endif

#ifndef LOG_H
#include <Log.h>
#endif

#define DAY_FILE_H

class DayFile : public JsonFile
{
    using json = nlohmann::json;

private:
    struct status
    {
        std::string name;
        std::string message;
        std::string start_time;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(status, name, message, start_time)
    } status;

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
        bool is_tracking;
        struct status status;
        std::vector<struct timeline_element> timeline;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(schema, date, is_tracking, status, timeline)
    } schema;

    uint diff_time_min(std::string, std::string);
    std::string avg_time_hrs(uint);
    std::string sum_day_task_avg_hrs(std::string, bool);
    uint sum_day_task_minutes(std::string, bool);

public:
    DayFile(const std::string day_files_path) : JsonFile(day_files_path + "/" + DayFile::get_current_date() + ".json"){};

    void load_day();
    void save_day();

    std::string start_tracking(std::string);
    std::string stop_tracking();
    std::string get_status();
    void start_break();
    void end_break();
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

uint DayFile::diff_time_min(std::string time1, std::string time2) // time is allways in HH:MM
{
    using namespace std;
    // string t1_hrs = time1.substr(0, 2);
    // string t1_min = time1.substr(3, 2);
    // string t2_hrs = time2.substr(0, 2);
    // string t2_min = time2.substr(3, 2);
    uint min_from_hrs = (stoi(time2.substr(0, 2)) - stoi(time1.substr(0, 2))) * 60;
    return stoi(time2.substr(3, 2)) + min_from_hrs - stoi(time1.substr(3, 2));
}

// get averged time in hours (in 0.25h intervals)
std::string DayFile::avg_time_hrs(uint minutes)
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

uint DayFile::sum_day_task_minutes(std::string name, bool include_current = false)
{
    uint min_sum = 0;
    for (auto it : schema.timeline)
    {
        if (it.name == name)
            min_sum += it.minutes;
    }
    if (include_current && (schema.status.name == name))
    {
        min_sum += diff_time_min(schema.status.start_time, DayFile::get_current_time());
    }
    return min_sum;
}

std::string DayFile::sum_day_task_avg_hrs(std::string name, bool include_current = false)
{
    return avg_time_hrs(sum_day_task_minutes(name, include_current));
}

std::string DayFile::stop_tracking()
{
    std::string ret_status;
    if (!schema.status.name.empty())
    {
        std::string stop_time = DayFile::get_current_time();
        std::string task_name = schema.status.name;
        uint task_time_min = diff_time_min(schema.status.start_time, stop_time);

        struct DayFile::timeline_element tmp_timeline_el = {
            .name = task_name,
            .minutes = task_time_min,
            .start_time = schema.status.start_time,
            .end_time = stop_time,
        };
        schema.timeline.emplace_back(tmp_timeline_el);

        ret_status = "Stopping to track " + task_name + ", for " + std::to_string(task_time_min) + " minutes\n";

        schema.status.name = "";
        schema.status.message = "Not tracking anything at the moment";
        schema.status.start_time = "";
    }
    else
    {
        ret_status = "Not tracking anything at the moment, nothing to do\n";
    }

    return ret_status;
}

std::string DayFile::start_tracking(std::string task_name)
{
    std::string ret_status;
    if (schema.status.name == task_name)
    {
        ret_status = "Already tracking " + task_name + " for " + std::to_string(diff_time_min(schema.status.start_time, DayFile::get_current_time())) + "\n";
    }
    else if (schema.status.name.empty())
    {
        ret_status = "Start tracking " + task_name + "\n";

        schema.is_tracking = true;
        schema.status.name = task_name;
        schema.status.start_time = DayFile::get_current_time();
        schema.status.message = "Currently tracking " + task_name + " since " + schema.status.start_time;
    }
    else // currently tracking different task
    {
        ret_status = stop_tracking();
        ret_status += start_tracking(task_name);
    }

    return ret_status;
}

std::string DayFile::get_status()
{
    std::string ret_string;
    ret_string += schema.status.message + "\n";

    if (!schema.status.name.empty())
    {
        ret_string += ("Today's total: " + std::to_string(sum_day_task_minutes(schema.status.name, true)) + " minutes");
    }

    return ret_string;
}
