#include <Log.h>
#include <rang.hpp>
#include <CLI11.hpp>
#include <unistd.h>
#include <algorithm>

#ifndef CONFIG_FILE_H
#include "ConfigFile.hpp"
#endif
#ifndef DAY_FILE_H
#include "DayFile.hpp"
#endif
#ifndef STATE_FILE_H
#include "StateFile.hpp"
#endif

#include "Pomodoro.hpp"

#ifndef APP_VERSION
#define APP_VERSION "1.0"
#endif

#ifndef STATE_DIR
#define STATE_DIR ".task_tracker"
#endif
#ifndef CONFIG_FILE_NAME
#define CONFIG_FILE_NAME "config.json"
#endif
#ifndef STATE_FILE_NAME
#define STATE_FILE_NAME "state.json"
#endif
#ifndef DAY_FILE_DIR
#define DAY_FILE_DIR "history"
#endif

int main(int argc, char const *argv[])
{
    using namespace std;

    const string state_path = (string)getenv("HOME") + "/" + STATE_DIR;

    ConfigFile config_file(state_path + "/" + CONFIG_FILE_NAME);
    config_file.load_config();

    // --- cli config ---

    CLI::App cli{"task-tracker++: count time spent on specific tasks or practice the Pomodoro technique"};
    cli.set_version_flag("-v, --version", "task-tracker++ by Wojciech Kopański\nversion " APP_VERSION);
    cli.set_help_all_flag("--help-all", "Expand all help");

    // track status
    auto &cli_status = *cli.add_subcommand("status", "Display current tracking status");

    // track add [task name]
    auto &cli_add = *cli.add_subcommand("add", "Add new task name to be able to track it");
    string add_task_name;
    cli_add.add_option("task name", add_task_name, "Name of the new task or project, e.g. reading")->required();

    // track stop
    auto &cli_stop = *cli.add_subcommand("stop", "Stop tracking the current task");

    // track stats [date]
    auto &cli_stats = *cli.add_subcommand("stats", "Get task statistics from the selected day");
    string stats_date = DayFile::get_current_date();
    cli_stats.add_option("date", stats_date, "Date in format YYYY-MM-DD, defaults to today (" + stats_date + ")");

    // track -p
    bool flag_pomodoro;
    cli.add_flag("-p,--pomodoro", flag_pomodoro, "Display live timer on current task instead of quitting");

    // track [task name]
    string task_name = "";
    string available_tasks = "";
    for (string it : config_file.get_projects())
    {
        available_tasks += "\n\t- " + it;
    }
    cli.add_option("task name", task_name, "Start tracking one of existing tasks:" + available_tasks);

    CLI11_PARSE(cli, argc, argv);

    // --- handle cli ---
    // the following doesn't run for --help or --version
    // using namespace rang;

    DayFile day_file(state_path + "/" + DAY_FILE_DIR, DayFile::get_current_date()); // DAY_FILE_DIR
    day_file.load_day();
    StateFile state_file(state_path + "/" + STATE_FILE_NAME, day_file); //
    state_file.load_state();

    if (cli_add)
    {
        if (config_file.add_project(add_task_name))
        {
            cout << "Added " << rang::fg::green << add_task_name << rang::fg::reset << " to available tasks!" << endl
                 << endl;
            config_file.save_config();
        }
        else
        {
            cout << rang::fg::red << "Error: " << add_task_name << " already exists in config! " << rang::fg::reset << endl
                 << endl;
        }
    }
    else if (cli_stats)
    {
        if (stats_date == DayFile::get_current_date())
        {
            cout << state_file.get_stats() << endl;
        }
        else
        {
            DayFile historical_day_file(state_path + "/" + DAY_FILE_DIR, stats_date); // DAY_FILE_DIR
            historical_day_file.load_day();
            cout << historical_day_file.get_stats() << endl;
        }
    }
    else if (cli_status)
    {
        cout << state_file.get_status() << endl;
    }
    else if (cli_stop)
    {
        cout << state_file.stop_tracking() << endl;
        state_file.save_state();
        day_file.save_day();
    }
    else if (!task_name.empty())
    {
        auto projects = config_file.get_projects();
        if (find(projects.begin(), projects.end(), task_name) != projects.end())
        {
            state_file.start_tracking(task_name);
            cout << "Now tracking " << rang::fg::cyan << task_name << rang::fg::reset << endl
                 << endl;
            state_file.save_state();
            day_file.save_day();
        }
        else
        {
            cout << rang::fg::red << "Error: " << task_name << " doesn't exist in config! Try: 'track add " << task_name << "'" << rang::fg::reset << endl
                 << endl;
        }
    }

    if (!flag_pomodoro)
        return 0;

    // -- interactive status ---

    pom::interactive_status(state_file);

    return 0;
}
