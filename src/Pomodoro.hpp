#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <thread>
#include <rang.hpp>

#ifndef STATE_FILE_H
#include "StateFile.hpp"
#endif

namespace pom
{
    static volatile bool keep_running = true;

    void user_input()
    {
        while (keep_running)
        {
            if (std::getchar())
            {
                keep_running = false;
            }
        }
    }

    void interactive_status(StateFile &state_file)
    {
        std::cout << std::endl;
        std::thread user_input_thread(user_input);
        std::string status = "";
        while (keep_running)
        {
            status = state_file.get_status_oneline() + '\r';
            std::cout << rang::fg::green << rang::style::bold << status << rang::fg::reset << rang::style::reset;
            std::cout.flush();
            sleep(1);
        }

        user_input_thread.join();
        std::cout << rang::fg::reset << rang::style::reset << std::endl;
    }
};
