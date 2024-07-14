#ifndef FRAG_Z_GAME_LOOP_HPP
#define FRAG_Z_GAME_LOOP_HPP

#include <functional>
#include "sbpt_generated_includes.hpp"

class GameLoopStats {
  public:
    Stopwatch non_rate_limited_stopwatch;
    Stopwatch fixed_timestep_stopwatch;
};

class GameLoop {
  public:
    void start(double update_rate_hz, const std::function<void(double)> &fixed_timestep_update_func,
               const std::function<void()> &non_rate_limited_update_func,
               const std::function<int()> &termination_condition_func);
    GameLoopStats game_loop_stats;
};

#endif // FRAG_Z_GAME_LOOP_HPP
