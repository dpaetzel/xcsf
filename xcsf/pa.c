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
 * @file pa.c
 * @author Richard Preen <rpreen@gmail.com>
 * @copyright The Authors.
 * @date 2015--2020.
 * @brief Prediction array functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "xcsf.h"
#include "utils.h"
#include "cl.h"
#include "pa.h"

/**
 * @brief Initialises the prediction array.
 * @param xcsf The XCSF data structure.
 */
void pa_init(XCSF *xcsf)
{
    xcsf->pa = malloc(sizeof(double) * xcsf->n_actions);
    xcsf->nr = malloc(sizeof(double) * xcsf->n_actions);
}

/**
 * @brief Builds the prediction array for the specified input.
 * @param xcsf The XCSF data structure.
 * @param x The input state.
 */
void pa_build(const XCSF *xcsf, const double *x)
{
    const SET *set = &xcsf->mset;
    double *pa = xcsf->pa;
    double *nr = xcsf->nr;
    int pa_size = xcsf->n_actions;
    for(int i = 0; i < pa_size; i++) {
        pa[i] = 0;
        nr[i] = 0;
    }
#ifdef PARALLEL_PRED
    CL **clist = malloc(sizeof(CL*) * set->size);
    CLIST *iter = set->list;
    for(int i = 0; i < set->size; i++) {
        clist[i] = NULL;
        if(iter != NULL) {
            clist[i] = iter->cl;
            iter = iter->next;
        }
    }
    #pragma omp parallel for reduction(+:pa[:pa_size],nr[:pa_size])
    for(int i = 0; i < set->size; i++) {
        if(clist[i] != NULL) {
            const double *predictions = cl_predict(xcsf, clist[i], x);
            pa[clist[i]->action] += predictions[0] * clist[i]->fit;
            nr[clist[i]->action] += clist[i]->fit;
        }
    }
    free(clist);
#else
    for(const CLIST *iter = set->list; iter != NULL; iter = iter->next) {
        const double *predictions = cl_predict(xcsf, iter->cl, x);
        pa[iter->cl->action] += predictions[0] * iter->cl->fit;
        nr[iter->cl->action] += iter->cl->fit;
    }
#endif
    for(int i = 0; i < xcsf->n_actions; i++) {
        if(nr[i] != 0) {
            pa[i] /= nr[i];
        } else {
            pa[i] = 0;
        }
    }
}

/**
 * @brief Returns the best action in the prediction array.
 * @param xcsf The XCSF data structure.
 * @return The best action.
 */
int pa_best_action(const XCSF *xcsf)
{
    int action = 0;
    for(int i = 1; i < xcsf->n_actions; i++) {
        if(xcsf->pa[action] < xcsf->pa[i]) {
            action = i;
        }
    }
    return action;
}

/**
 * @brief Returns a random action from the prediction array.
 * @param xcsf The XCSF data structure.
 * @return A random action.
 */
int pa_rand_action(const XCSF *xcsf)
{
    int action = 0;
    do {
        action = irand_uniform(0, xcsf->n_actions);
    } while(xcsf->nr[action] == 0);
    return action;
}

/**
 * @brief Returns the highest value in the prediction array.
 * @param xcsf The XCSF data structure.
 * @return The highest value in the prediction array.
 */
double pa_best_val(const XCSF *xcsf)
{
    double max = xcsf->pa[0];
    for(int i = 1; i < xcsf->n_actions; i++) {
        if(max < xcsf->pa[i]) {
            max = xcsf->pa[i];
        }
    }
    return max;
}

/**
 * @brief Returns the value of a specified action in the prediction array.
 * @param xcsf The XCSF data structure.
 * @param action The specified action.
 * @return The value of the action in the prediction array.
 */
double pa_val(const XCSF *xcsf, int action)
{
    if(action >= 0 && action < xcsf->n_actions) {
        return xcsf->pa[action];
    }
    printf("pa_val() error: invalid action specified: %d\n", action);
    exit(EXIT_FAILURE);
}

/**
 * @brief Frees the prediction array.
 * @param xcsf The XCSF data structure.
 */
void pa_free(const XCSF *xcsf)
{
    free(xcsf->pa);
    free(xcsf->nr);
}
