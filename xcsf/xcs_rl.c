/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file xcs_rl.c
 * @author Richard Preen <rpreen@gmail.com>
 * @copyright The Authors.
 * @date 2015--2020.
 * @brief Reinforcement learning functions.
 * @details A trial consists of one or more steps.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>
#include "xcsf.h"
#include "utils.h"
#include "param.h"
#include "cl.h"
#include "clset.h"
#include "pa.h"
#include "ea.h"
#include "perf.h"
#include "xcs_rl.h"
#include "env.h"

static double xcs_rl_trial(XCSF *xcsf, double *error, _Bool explore);

/**
 * @brief Executes a reinforcement learning experiment.
 * @param xcsf The XCSF data structure.
 * @return The mean number of steps to goal.
 */
double xcs_rl_exp(XCSF *xcsf)
{
    pa_init(xcsf);
    double error = 0; // prediction error: individual trial
    double werr = 0; // prediction error: windowed total
    double tperf = 0; // steps to goal: total over all trials
    double wperf = 0; // steps to goal: windowed total
    for(int cnt = 0; cnt < xcsf->MAX_TRIALS; cnt++) {
        xcs_rl_trial(xcsf, &error, true); // explore
        double perf = xcs_rl_trial(xcsf, &error, false); // exploit
        wperf += perf;
        tperf += perf;
        werr += error;
        perf_print(xcsf, &wperf, &werr, cnt);
    }
    pa_free(xcsf);
    return tperf / xcsf->MAX_TRIALS;
}

/**
 * @brief Executes a reinforcement learning trial using a built-in environment.
 * @param xcsf The XCSF data structure.
 * @param error The mean system prediction error (set by this function).
 * @param explore Whether this is an exploration or exploitation trial.
 * @return Returns the accuracy for single-step problems and the number of
 * steps taken to reach the goal for multi-step problems.
 */
static double xcs_rl_trial(XCSF *xcsf, double *error, _Bool explore)
{
    env_reset(xcsf);
    param_set_explore(xcsf, explore);
    xcs_rl_init_trial(xcsf);
    *error = 0; // mean prediction error over all steps taken
    double reward = 0;
    _Bool reset = false;
    int steps = 0;
    while(steps < xcsf->TELETRANSPORTATION && !reset) {
        xcs_rl_init_step(xcsf);
        const double *state = env_get_state(xcsf);
        int action = xcs_rl_decision(xcsf, state);
        reward = env_execute(xcsf, action);
        reset = env_is_reset(xcsf);
        xcs_rl_update(xcsf, state, action, reward, reset);
        *error += xcs_rl_error(xcsf, action, reward, reset, env_max_payoff(xcsf));
        xcs_rl_end_step(xcsf, state, action, reward);
        steps++;
    }
    xcs_rl_end_trial(xcsf);
    *error /= steps;
    if(!env_multistep(xcsf)) {
        return (reward > 0) ? 1 : 0;
    }
    return steps;
}

/**
 * @brief Initialises a reinforcement learning trial.
 * @param xcsf The XCSF data structure.
 */
void xcs_rl_init_trial(XCSF *xcsf)
{
    xcsf->prev_reward = 0;
    xcsf->prev_pred = 0;
    if(xcsf->x_dim < 1) { // memory allocation guard
        printf("xcs_rl_init_trial(): error x_dim less than 1\n");
        xcsf->x_dim = 1;
        exit(EXIT_FAILURE);
    }
    xcsf->prev_state = malloc(xcsf->x_dim * sizeof(double));
    clset_init(&xcsf->prev_aset);
    clset_init(&xcsf->kset);
}

/**
 * @brief Frees memory used by a reinforcement learning trial.
 * @param xcsf The XCSF data structure.
 */
void xcs_rl_end_trial(XCSF *xcsf)
{
    clset_free(&xcsf->prev_aset);
    clset_kill(xcsf, &xcsf->kset);
    free(xcsf->prev_state);
}

/**
 * @brief Initialises a step in a reinforcement learning trial.
 * @param xcsf The XCSF data structure.
 */
void xcs_rl_init_step(XCSF *xcsf)
{
    clset_init(&xcsf->mset);
    clset_init(&xcsf->aset);
}

/**
 * @brief Ends a step in a reinforcement learning trial.
 * @param xcsf The XCSF data structure.
 * @param state The current input state.
 * @param action The current action.
 * @param reward The current reward.
 */
void xcs_rl_end_step(XCSF *xcsf, const double *state, int action, double reward)
{
    clset_free(&xcsf->mset);
    clset_free(&xcsf->prev_aset);
    xcsf->prev_aset = xcsf->aset;
    xcsf->prev_reward = reward;
    xcsf->prev_pred = pa_val(xcsf, action);
    memcpy(xcsf->prev_state, state, sizeof(double) * xcsf->x_dim);
}

/**
 * @brief Provides reinforcement to the sets.
 * @details Creates the action set, updates the classifiers and runs the EA.
 * @param xcsf The XCSF data structure.
 * @param state The input state.
 * @param action The action selected.
 * @param reward The reward from performing the action.
 * @param reset Whether the environment is in the reset state.
 */
void xcs_rl_update(XCSF *xcsf, const double *state, int action, double reward,
                   _Bool reset)
{
    // create action set
    clset_action(xcsf, action);
    // update previous action set and run EA
    if(xcsf->prev_aset.list != NULL) {
        double payoff = xcsf->prev_reward + (xcsf->GAMMA * pa_best_val(xcsf));
        clset_validate(&xcsf->prev_aset);
        clset_update(xcsf, &xcsf->prev_aset, xcsf->prev_state, &payoff, false);
        if(xcsf->explore) {
            ea(xcsf, &xcsf->prev_aset);
        }
    }
    // in goal state: update current action set and run EA
    if(reset) {
        clset_validate(&xcsf->aset);
        clset_update(xcsf, &xcsf->aset, state, &reward, true);
        if(xcsf->explore) {
            ea(xcsf, &xcsf->aset);
        }
    }
}

/**
 * @brief Returns the reinforcement learning system prediction error.
 * @param xcsf The XCSF data structure.
 * @param action The current action.
 * @param reward The current reward.
 * @param reset The current reset status.
 * @param max_p The maximum payoff in the environment.
 * @return The prediction error.
 */
double xcs_rl_error(const XCSF *xcsf, int action, double reward, _Bool reset,
                    double max_p)
{
    double error = 0;
    if(xcsf->prev_aset.list != NULL) {
        error += fabs(xcsf->GAMMA * pa_val(xcsf, action)
                      + xcsf->prev_reward - xcsf->prev_pred) / max_p;
    }
    if(reset) {
        error += fabs(reward - pa_val(xcsf, action)) / max_p;
    }
    return error;
}

/**
 * @brief Selects an action to perform in a reinforcement learning problem.
 * @details Constructs the match set and selects an action to perform.
 * @param xcsf The XCSF data structure.
 * @param state The input state.
 * @return The selected action.
 */
int xcs_rl_decision(XCSF *xcsf, const double *state)
{
    clset_match(xcsf, state);
    pa_build(xcsf, state);
    if(xcsf->explore && rand_uniform(0, 1) < xcsf->P_EXPLORE) {
        return pa_rand_action(xcsf);
    }
    return pa_best_action(xcsf);
}