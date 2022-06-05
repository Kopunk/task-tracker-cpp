#include <vector>

#ifndef JSON_FILE_H
#include "JsonFile.hpp"
#endif

#ifndef DAY_FILE_H
#include "DayFile.hpp"
#endif

#ifndef LOG_H
#include <Log.h>
#endif

#define STATE_FILE_H

class StateFile : public JsonFile
{
    using json = nlohmann::json;

private:
    DayFile &day_file;

    struct status
    {
        std::string name;
        std::string message;
        std::string start_time;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(status, name, message, start_time)
    } status;

    struct schema
    {
        std::string date;
        bool is_tracking;
        struct status status;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(schema, date, is_tracking, status)
    } schema;

    uint diff_time_min(std::string, std::string);
    std::string sum_day_task_avg_hrs(std::string);
    uint sum_day_task_minutes(std::string);

public:
    StateFile(const std::string state_file_path, DayFile &day_file) : JsonFile(state_file_path), day_file(day_file){};

    void load_state();
    void save_state();

    std::string start_tracking(std::string);
    std::string stop_tracking();
    std::string get_status();
    std::string get_status_oneline();
    std::string get_stats();
};

void StateFile::load_state()
{
    try
    {
        load_file();
    }
    catch (const nlohmann::detail::parse_error &e)
    {
        plog::log("error while reading the file " + this->file_path);
        save_state();
        load_file();
    }

    plog::log("load day file at " + this->file_path + ":\n" + file_json.dump(JSON_INDENT));
    schema = file_json;
}

void StateFile::save_state()
{
    schema.date = StateFile::get_current_date();
    file_json = schema;
    plog::log("saving day file at " + this->file_path + ":\n" + file_json.dump(JSON_INDENT));
    save_file();
}

uint StateFile::diff_time_min(std::string time1, std::string time2) // time is allways in HH:MM
{
    using namespace std;
    if (time1.length() != 5 || time2.length() != 5)
        return 0;
    string t1_hrs = time1.substr(0, 2);
    string t1_min = time1.substr(3, 2);
    string t2_hrs = time2.substr(0, 2);
    string t2_min = time2.substr(3, 2);

    uint t1 = stoi(t1_hrs) * 60 + stoi(t1_min);
    uint t2 = stoi(t2_hrs) * 60 + stoi(t2_min);

    if (t1 > t2) // handle working during the midnight
        return t2 + ((60 * 24) - t1);

    return t2 - t1;
}

// differs from DayFile::sum_day_task_minutes by including current state
uint StateFile::sum_day_task_minutes(std::string name)
{
    uint min_sum = 0;
    for (auto it : day_file.schema.timeline)
    {
        if (it.name == name)
            min_sum += it.minutes;
    }
    if ((schema.status.name == name))
    {
        min_sum += diff_time_min(schema.status.start_time, StateFile::get_current_time());
    }
    return min_sum;
}

// differs from DayFile::sum_day_task_avg_hrs by including current state
std::string StateFile::sum_day_task_avg_hrs(std::string name)
{
    return avg_time_hrs(sum_day_task_minutes(name));
}

std::string StateFile::stop_tracking()
{
    std::string ret_status;
    if (!schema.status.name.empty())
    {
        std::string stop_time = StateFile::get_current_time();
        std::string task_name = schema.status.name;
        uint task_time_min = diff_time_min(schema.status.start_time, stop_time);

        struct DayFile::timeline_element tmp_timeline_el = {
            .name = task_name,
            .minutes = task_time_min,
            .start_time = schema.status.start_time,
            .end_time = stop_time,
        };
        day_file.schema.timeline.emplace_back(tmp_timeline_el);
        day_file.save_day();

        ret_status = "Stopping to track " + task_name + ", for " + std::to_string(task_time_min) + " minutes\n";

        schema.status.name = "";
        schema.status.message = "Not tracking anything at the moment";
        schema.status.start_time = "";
        schema.is_tracking = false;
    }
    else
    {
        ret_status = "Not tracking anything at the moment, nothing to do\n";
    }

    return ret_status;
}

std::string StateFile::start_tracking(std::string task_name)
{
    std::string ret_status;
    if (schema.status.name == task_name)
    {
        ret_status = "Already tracking " + task_name + " for " + std::to_string(diff_time_min(schema.status.start_time, StateFile::get_current_time())) + "\n";
    }
    else if (schema.status.name.empty())
    {
        ret_status = "Start tracking " + task_name + "\n";

        schema.is_tracking = true;
        schema.status.name = task_name;
        schema.status.start_time = StateFile::get_current_time();
        schema.status.message = "Currently tracking " + task_name + " since " + schema.status.start_time;
    }
    else // currently tracking different task
    {
        ret_status = stop_tracking();
        ret_status += start_tracking(task_name);
    }

    return ret_status;
}

std::string StateFile::get_status()
{
    std::string ret_string;
    ret_string += schema.status.message + "\n";

    if (!schema.status.name.empty())
    {
        ret_string += ("Today's total: " + std::to_string(sum_day_task_minutes(schema.status.name)) + " minutes\n");
    }

    return ret_string;
}

std::string StateFile::get_status_oneline()
{
    if (!schema.is_tracking)
        return "Not tracking anything, Enter to exit";
    return "Currently tracking " + schema.status.name + " for " + std::to_string(diff_time_min(schema.status.start_time, StateFile::get_current_time())) + " min, press Enter to exit.";
}

// returns stats including currently running tasks
std::string StateFile::get_stats()
{
    using namespace std;

    if (day_file.schema.timeline.empty())
    {
        if (!schema.is_tracking)
        {
            return "No records today";
        }
        else
        {
            string task_name = schema.status.name;
            string avg_time = sum_day_task_avg_hrs(task_name);
            string time_min = to_string(sum_day_task_minutes(task_name));
            task_name.append(32 - task_name.length(), ' ');
            return "\t- " + task_name + avg_time + "h" + "\t(" + time_min + " min)\n";
        }
    }

    string ret_str = "";
    set<string> projects;

    if (schema.is_tracking)
    {
        projects.insert(schema.status.name);
    }
    
    for (auto it : day_file.schema.timeline)
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
