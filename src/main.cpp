#include <Log.h>
#include <CLI11.hpp>
#include <unistd.h>

#ifndef APP_VERSION
#define APP_VERSION "0.1"
#endif

#ifndef STATE_DIR
#define STATE_DIR ".task_tracker"
#endif

#include "DayFile.hpp"
#include "ConfigFile.hpp"


// void cli_result_handler(bool flag_pomodoro, bool flag_status, std::string flag_add_task, bool flag_stop, std::string flag_stats_day, std::string task_name)
// {
// }

int main(int argc, char const *argv[])
{
    using namespace std;

    const string state_path = (string)getenv("HOME") + "/" + STATE_DIR;

    ConfigFile config_file("TEST_CONFIG_FILE.json");
    config_file.load_config();
    config_file.add_project("mynewawesomeproj");
    config_file.save_config();

    DayFile day_file("history");
    day_file.load_day();

    cout << day_file.start_tracking("mynewawesomeproj") << endl;
    cout << day_file.stop_tracking() << endl;
    cout << day_file.start_tracking("test_tast_1") << endl;
    cout << day_file.start_tracking("test_tast_2") << endl;

    cout << day_file.get_status() << endl;

    day_file.save_day();


    // CLI::App cli{"task-tracker++: count time spent on specific tasks or practice the Pomodoro technique"};

    // cli.set_version_flag("-v, --version", "task-tracker++ by Wojciech Kopański\nversion " APP_VERSION);

    // bool flag_pomodoro;
    // cli.add_flag("-p,--pomodoro", flag_pomodoro, "Display live timer on current task");

    // bool flag_status;
    // cli.add_flag("-i,--info,--status", flag_status, "Display current tracking status");

    // string flag_add_task;
    // cli.add_option("-a,--add,--add-task", flag_add_task, "Add new task name to track it");

    // bool flag_stop;
    // cli.add_flag("-s,--stop", flag_stop, "Stop tracking the current task");

    // // bool flag_break;
    // // cli.add_flag("-b,--break", flag_break, "Take a break!");
    // // bool flag_resume;
    // // cli.add_flag("-r,--resume", flag_resume, "Resume tracking the task that you took a break from");

    // string flag_stats_day = DayFile::get_current_date();
    // cli.add_option("-S,--stats", flag_stats_day, "Get task statistics from the selected day (format: YYYY-MM-DD), defaults to today (" + DayFile::get_current_date() + ")");

    // string task_name;
    // string available_tasks = "";
    // for (string it : config_file.get_projects())
    // {
    //     available_tasks += "\n\t- " + it;
    // }
    // cli.add_option("task name", task_name, "Start tracking one of existing tasks:" + available_tasks);

    // CLI11_PARSE(cli, argc, argv);




    // cli_result_handler(flag_pomodoro, flag_status, flag_add_task, flag_stop, flag_stats_day, task_name);

    // std::cout << '-' << std::flush;
    // for (;;)
    // {
    //     sleep(1);
    //     std::cout << "\b\\" << std::flush;
    //     sleep(1);
    //     std::cout << "\b|" << std::flush;
    //     sleep(1);
    //     std::cout << "\b/" << std::flush;
    //     sleep(1);
    //     std::cout << "\b-" << std::flush;
    // }

    return 0;
}
