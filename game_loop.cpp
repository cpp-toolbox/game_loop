#include <cstdlib>
#include <chrono>
#include "GLFW/glfw3.h"
#include "game_loop.hpp"

/* game loop
 *
 * brief:
 * 		a game loop that updates the game state at fixed time intervals while also rendering as fast as
 * possible.
 *
 * description:
 *      in general we have an update function, it works based on the idea that a certain amount of real-life time has
 *      occurred, one way of doing this would be to simply compute the amount of time that has passed since the last
 * iteration and use that as our delta time, but we have some constraints saying that our update function only works
 * correctly when the delta time value is within some range.
 *
 *      If your update function uses physics, then usually a good range is something like 20-60Hz, therefore after
 * choosing a fixed timestep that would keep your update function behaving as desired then we only will call the update
 * function if that much real-world time has passed.
 *
 *      We do this by having a variable which stores how much time has elapsed since the last update, and only calling
 * update if enough time has passed.
 *
 * notes:
 * 		all time units are seconds
 *
 *  author:
 *  	cuppajoeman (2023)
 */

void GameLoop::start(double update_rate_hz, const std::function<void(double)> &fixed_timestep_update_func,
                     const std::function<void()> &non_rate_limited_update_func,
                     const std::function<int()> &termination_condition_func) {

    double time_elapsed_since_start_of_program = 0;

    // 1/N seconds per iteration
    double time_between_state_update = 1.0 / update_rate_hz;

    double time_elapsed_since_last_state_update = 0;

    bool first_iteration = true;

    double time_at_start_of_iteration_last_iteration = -1.0;
    double duration_of_last_iteration = -1.0;

    while (!termination_condition_func()) {

        double time_at_start_of_iteration = glfwGetTime(); // (T)

        if (first_iteration) {
            // The last few lines of this iteration are next loops last iteration.
            first_iteration = false;
            time_at_start_of_iteration_last_iteration = time_at_start_of_iteration; // (C)
            time_elapsed_since_last_state_update =
                time_at_start_of_iteration; // (F): Pretend an update has occurred at time 0 for bootstrapping purposes
            continue;
        }

        // Note that this measures how long it takes for the code to start at (T) and arrive back at (T),
        // (G): Due to (C) tesli == 0 on the second iteration, and non-zero after that, this doesn't cause any issues.
        duration_of_last_iteration = time_at_start_of_iteration - time_at_start_of_iteration_last_iteration;

        // None of the updates that could have happened during the last iteration have been applied
        // This is because last iteration, we retroactively applied last last iterations updates
        time_elapsed_since_last_state_update += duration_of_last_iteration;

        // since the value of teslsu is only updated by (E), this would always be false, but (F) bootstraps the process
        bool enough_time_for_updates = time_elapsed_since_last_state_update >= time_between_state_update;

        // Due to the (G), an update could only happen starting from the 3rd iteration
        if (enough_time_for_updates) {

            // retroactively apply updates that should have occurred during previous iterations
            double time_remaining_to_fit_updates = time_elapsed_since_last_state_update;
            bool enough_time_to_fit_update = true;

            while (enough_time_to_fit_update) {

                fixed_timestep_update_func(time_between_state_update);
                game_loop_stats.fixed_timestep_stopwatch.press();

                time_remaining_to_fit_updates -= time_between_state_update;
                enough_time_to_fit_update = time_remaining_to_fit_updates >= time_between_state_update;
            }
            time_elapsed_since_last_state_update = time_remaining_to_fit_updates;
        }

        non_rate_limited_update_func();
        game_loop_stats.non_rate_limited_stopwatch.press();

        // With respect to the start of the next iteration, the code down here is previous iteration.
        time_at_start_of_iteration_last_iteration = time_at_start_of_iteration;
    }
}

void Stopwatch::press() {
    if (first_time) {
        previous_time = std::chrono::system_clock::now();
        first_time = false;
    } else {
        std::chrono::time_point<std::chrono::system_clock> current_time = std::chrono::system_clock::now();
        std::chrono::duration<double> delta = current_time - previous_time;
        times[curr_idx] = delta.count();
        curr_idx = (curr_idx + 1) % num_times_to_average_over; // circular clobbering array.
        previous_time = current_time;
    }
    average_frequency = 1.0 / this->compute_average_period();
}

double Stopwatch::compute_average_period() {
    double delta_sum = 0.0;
    for (int i = 0; i < num_times_to_average_over; i++) {
        delta_sum += times[i];
    }
    return delta_sum / (double)num_times_to_average_over;
}
