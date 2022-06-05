# task-tracker++

count time spent on specific tasks or practice the Pomodoro technique

## Installation

> Ensure you have CMake and Make installed

```
git clone https://github.com/Kopunk/task-tracker-cpp.git
cd task-tracker-cpp
./install.sh
# Follow instructions to add executable to PATH

track --version
```

> ℹ️ Configuration and binary is installed to single direcotry in user's home directory


## Usage

```
$ track --help-all

task-tracker++: count time spent on specific tasks or practice the Pomodoro technique
Usage: ./bin/track [OPTIONS] [task name] [SUBCOMMAND]

Positionals:
task name TEXT                  Start tracking one of existing tasks:
                                 - your
                                 - tasks
                                 - will
                                 - display
                                 - here

Options:
-h,--help                       Print this help message and exit
-v,--version                    Display program version information and exit
--help-all                      Expand all help
-p,--pomodoro                   Display live timer on current task instead of quitting

Subcommands:
status                          Display current tracking status
add                             Add new task name to be able to track it
stop                            Stop tracking the current task
stats                           Get task statistics from the selected day

Subcommands:
status
  Display current tracking status

add
  Add new task name to be able to track it
  Positionals:
    task name TEXT REQUIRED     Name of the new task or project, e.g. reading

stop
  Stop tracking the current task

stats
  Get task statistics from the selected day
  Positionals:
    date TEXT                   Date in format YYYY-MM-DD, defaults to today (2022-06-05)

```

## Used libraries

- [json](https://github.com/nlohmann/json)
- [rang](https://github.com/agauniyal/rang)
- [CLI11](https://github.com/CLIUtils/CLI11)
