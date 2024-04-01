#ifndef FRAG_Z_GAME_LOOP_HPP
#define FRAG_Z_GAME_LOOP_HPP

#include <functional>

/**
 * \brief A stopwatch which measures how frequently it's been pressed in seconds
 * to be used to measure at what frequency a specific line of code is run at.
 */
class Stopwatch {
public:
    void press();
    double average_frequency = 0.0;
private:
    double compute_average_period();
    int num_times_to_average_over = 10;
    double times[10] = {0};
    bool first_time = true;
    int curr_idx = 0;
    std::chrono::time_point<std::chrono::system_clock> previous_time;
};

class GameLoopStats {
public:
    Stopwatch non_rate_limited_stopwatch;
    Stopwatch fixed_timestep_stopwatch;
};

class GameLoop {
public:
    void start(
            double update_rate_hz,
            const std::function<void(double)>& fixed_timestep_update_func,
            const std::function<void()>& non_rate_limited_update_func,
            const std::function<int()>& termination_condition_func
    );
    GameLoopStats game_loop_stats;
};


#endif //FRAG_Z_GAME_LOOP_HPP