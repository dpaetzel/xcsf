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
 * @file param.c
 * @author Richard Preen <rpreen@gmail.com>
 * @copyright The Authors.
 * @date 2015--2020.
 * @brief Functions for setting and printing parameters.
 */

#include "param.h"

#ifdef PARALLEL
    #include <omp.h>
#endif

/**
 * @brief Initialises default XCSF general parameters.
 * @param xcsf The XCSF data structure.
 */
static void
param_defaults_general(struct XCSF *xcsf)
{
    param_set_omp_num_threads(xcsf, 8);
    param_set_pop_init(xcsf, true);
    param_set_max_trials(xcsf, 100000);
    param_set_perf_trials(xcsf, 1000);
    param_set_pop_size(xcsf, 2000);
    param_set_loss_func(xcsf, 0);
}

/**
 * @brief Prints XCSF general parameters.
 * @param xcsf The XCSF data structure.
 */
static void
param_print_general(const struct XCSF *xcsf)
{
    printf("OMP_NUM_THREADS=%d", xcsf->OMP_NUM_THREADS);
    printf(", POP_INIT=");
    xcsf->POP_INIT ? printf("true") : printf("false");
    printf(", MAX_TRIALS=%d", xcsf->MAX_TRIALS);
    printf(", PERF_TRIALS=%d", xcsf->PERF_TRIALS);
    printf(", POP_SIZE=%d", xcsf->POP_SIZE);
    printf(", LOSS_FUNC=%d", xcsf->LOSS_FUNC);
}

/**
 * @brief Saves XCSF general parameters.
 * @param xcsf The XCSF data structure.
 * @param fp Pointer to the output file.
 * @return The total number of elements written.
 */
static size_t
param_save_general(const struct XCSF *xcsf, FILE *fp)
{
    size_t s = 0;
    s += fwrite(&xcsf->OMP_NUM_THREADS, sizeof(int), 1, fp);
    s += fwrite(&xcsf->POP_INIT, sizeof(_Bool), 1, fp);
    s += fwrite(&xcsf->MAX_TRIALS, sizeof(int), 1, fp);
    s += fwrite(&xcsf->PERF_TRIALS, sizeof(int), 1, fp);
    s += fwrite(&xcsf->POP_SIZE, sizeof(int), 1, fp);
    s += fwrite(&xcsf->LOSS_FUNC, sizeof(int), 1, fp);
    return s;
}

/**
 * @brief Loads XCSF general parameters.
 * @param xcsf The XCSF data structure.
 * @param fp Pointer to the output file.
 * @return The total number of elements read.
 */
static size_t
param_load_general(struct XCSF *xcsf, FILE *fp)
{
    size_t s = 0;
    s += fread(&xcsf->OMP_NUM_THREADS, sizeof(int), 1, fp);
    s += fread(&xcsf->POP_INIT, sizeof(_Bool), 1, fp);
    s += fread(&xcsf->MAX_TRIALS, sizeof(int), 1, fp);
    s += fread(&xcsf->PERF_TRIALS, sizeof(int), 1, fp);
    s += fread(&xcsf->POP_SIZE, sizeof(int), 1, fp);
    s += fread(&xcsf->LOSS_FUNC, sizeof(int), 1, fp);
    loss_set_func(xcsf);
    return s;
}

/**
 * @brief Initialises default general classifier parameters.
 * @param xcsf The XCSF data structure.
 */
static void
param_defaults_cl_general(struct XCSF *xcsf)
{
    param_set_eps_0(xcsf, 0.01);
    param_set_alpha(xcsf, 0.1);
    param_set_nu(xcsf, 5);
    param_set_beta(xcsf, 0.1);
    param_set_delta(xcsf, 0.1);
    param_set_theta_del(xcsf, 20);
    param_set_init_fitness(xcsf, 0.01);
    param_set_init_error(xcsf, 0);
    param_set_err_reduc(xcsf, 1);
    param_set_fit_reduc(xcsf, 0.1);
    param_set_m_probation(xcsf, 10000);
    param_set_stateful(xcsf, true);
}

/**
 * @brief Prints XCSF general classifier parameters.
 * @param xcsf The XCSF data structure.
 */
static void
param_print_cl_general(const struct XCSF *xcsf)
{
    printf(", EPS_0=%f", xcsf->EPS_0);
    printf(", ALPHA=%f", xcsf->ALPHA);
    printf(", NU=%f", xcsf->NU);
    printf(", BETA=%f", xcsf->BETA);
    printf(", DELTA=%f", xcsf->DELTA);
    printf(", THETA_DEL=%d", xcsf->THETA_DEL);
    printf(", INIT_FITNESS=%f", xcsf->INIT_FITNESS);
    printf(", INIT_ERROR=%f", xcsf->INIT_ERROR);
    printf(", ERR_REDUC=%f", xcsf->ERR_REDUC);
    printf(", FIT_REDUC=%f", xcsf->FIT_REDUC);
    printf(", M_PROBATION=%d", xcsf->M_PROBATION);
    printf(", STATEFUL=");
    xcsf->STATEFUL ? printf("true") : printf("false");
}

/**
 * @brief Saves XCSF general classifier parameters.
 * @param xcsf The XCSF data structure.
 * @param fp Pointer to the output file.
 * @return The total number of elements written.
 */
static size_t
param_save_cl_general(const struct XCSF *xcsf, FILE *fp)
{
    size_t s = 0;
    s += fwrite(&xcsf->EPS_0, sizeof(double), 1, fp);
    s += fwrite(&xcsf->ALPHA, sizeof(double), 1, fp);
    s += fwrite(&xcsf->NU, sizeof(double), 1, fp);
    s += fwrite(&xcsf->BETA, sizeof(double), 1, fp);
    s += fwrite(&xcsf->DELTA, sizeof(double), 1, fp);
    s += fwrite(&xcsf->THETA_DEL, sizeof(int), 1, fp);
    s += fwrite(&xcsf->INIT_FITNESS, sizeof(double), 1, fp);
    s += fwrite(&xcsf->INIT_ERROR, sizeof(double), 1, fp);
    s += fwrite(&xcsf->ERR_REDUC, sizeof(double), 1, fp);
    s += fwrite(&xcsf->FIT_REDUC, sizeof(double), 1, fp);
    s += fwrite(&xcsf->M_PROBATION, sizeof(int), 1, fp);
    s += fwrite(&xcsf->STATEFUL, sizeof(_Bool), 1, fp);
    return s;
}

/**
 * @brief Loads XCSF general classifier parameters.
 * @param xcsf The XCSF data structure.
 * @param fp Pointer to the output file.
 * @return The total number of elements read.
 */
static size_t
param_load_cl_general(struct XCSF *xcsf, FILE *fp)
{
    size_t s = 0;
    s += fread(&xcsf->EPS_0, sizeof(double), 1, fp);
    s += fread(&xcsf->ALPHA, sizeof(double), 1, fp);
    s += fread(&xcsf->NU, sizeof(double), 1, fp);
    s += fread(&xcsf->BETA, sizeof(double), 1, fp);
    s += fread(&xcsf->DELTA, sizeof(double), 1, fp);
    s += fread(&xcsf->THETA_DEL, sizeof(int), 1, fp);
    s += fread(&xcsf->INIT_FITNESS, sizeof(double), 1, fp);
    s += fread(&xcsf->INIT_ERROR, sizeof(double), 1, fp);
    s += fread(&xcsf->ERR_REDUC, sizeof(double), 1, fp);
    s += fread(&xcsf->FIT_REDUC, sizeof(double), 1, fp);
    s += fread(&xcsf->M_PROBATION, sizeof(int), 1, fp);
    s += fread(&xcsf->STATEFUL, sizeof(_Bool), 1, fp);
    return s;
}

/**
 * @brief Initialises default multistep parameters.
 * @param xcsf The XCSF data structure.
 */
static void
param_defaults_multistep(struct XCSF *xcsf)
{
    param_set_gamma(xcsf, 0.95);
    param_set_teletransportation(xcsf, 50);
    param_set_p_explore(xcsf, 0.9);
}

/**
 * @brief Prints XCSF multistep parameters.
 * @param xcsf The XCSF data structure.
 */
static void
param_print_multistep(const struct XCSF *xcsf)
{
    printf(", GAMMA=%f", xcsf->GAMMA);
    printf(", TELETRANSPORTATION=%d", xcsf->TELETRANSPORTATION);
    printf(", P_EXPLORE=%f", xcsf->P_EXPLORE);
}

/**
 * @brief Saves multistep parameters.
 * @param xcsf The XCSF data structure.
 * @param fp Pointer to the output file.
 * @return The total number of elements written.
 */
static size_t
param_save_multistep(const struct XCSF *xcsf, FILE *fp)
{
    size_t s = 0;
    s += fwrite(&xcsf->GAMMA, sizeof(double), 1, fp);
    s += fwrite(&xcsf->TELETRANSPORTATION, sizeof(int), 1, fp);
    s += fwrite(&xcsf->P_EXPLORE, sizeof(double), 1, fp);
    return s;
}

/**
 * @brief Saves multistep parameters.
 * @param xcsf The XCSF data structure.
 * @param fp Pointer to the output file.
 * @return The total number of elements read.
 */
static size_t
param_load_multistep(struct XCSF *xcsf, FILE *fp)
{
    size_t s = 0;
    s += fread(&xcsf->GAMMA, sizeof(double), 1, fp);
    s += fread(&xcsf->TELETRANSPORTATION, sizeof(int), 1, fp);
    s += fread(&xcsf->P_EXPLORE, sizeof(double), 1, fp);
    return s;
}

/**
 * @brief Initialises default subsumption parameters.
 * @param xcsf The XCSF data structure.
 */
static void
param_defaults_subsumption(struct XCSF *xcsf)
{
    param_set_ea_subsumption(xcsf, false);
    param_set_set_subsumption(xcsf, false);
    param_set_theta_sub(xcsf, 100);
}

/**
 * @brief Prints XCSF subsumption parameters.
 * @param xcsf The XCSF data structure.
 */
static void
param_print_subsumption(const struct XCSF *xcsf)
{
    printf(", EA_SUBSUMPTION=");
    xcsf->EA_SUBSUMPTION ? printf("true") : printf("false");
    printf(", SET_SUBSUMPTION=");
    xcsf->SET_SUBSUMPTION ? printf("true") : printf("false");
    printf(", THETA_SUB=%d", xcsf->THETA_SUB);
}

/**
 * @brief Saves subsumption parameters.
 * @param xcsf The XCSF data structure.
 * @param fp Pointer to the output file.
 * @return The total number of elements written.
 */
static size_t
param_save_subsumption(const struct XCSF *xcsf, FILE *fp)
{
    size_t s = 0;
    s += fwrite(&xcsf->EA_SUBSUMPTION, sizeof(_Bool), 1, fp);
    s += fwrite(&xcsf->SET_SUBSUMPTION, sizeof(_Bool), 1, fp);
    s += fwrite(&xcsf->THETA_SUB, sizeof(int), 1, fp);
    return s;
}

/**
 * @brief Loads subsumption parameters.
 * @param xcsf The XCSF data structure.
 * @param fp Pointer to the output file.
 * @return The total number of elements read.
 */
static size_t
param_load_subsumption(struct XCSF *xcsf, FILE *fp)
{
    size_t s = 0;
    s += fread(&xcsf->EA_SUBSUMPTION, sizeof(_Bool), 1, fp);
    s += fread(&xcsf->SET_SUBSUMPTION, sizeof(_Bool), 1, fp);
    s += fread(&xcsf->THETA_SUB, sizeof(int), 1, fp);
    return s;
}

/**
 * @brief Initialises default evolutionary algorithm parameters.
 * @param xcsf The XCSF data structure.
 */
static void
param_defaults_ea(struct XCSF *xcsf)
{
    param_set_ea_select_type(xcsf, 0);
    param_set_ea_select_size(xcsf, 0.4);
    param_set_theta_ea(xcsf, 50);
    param_set_lambda(xcsf, 2);
    param_set_p_crossover(xcsf, 0.8);
}

/**
 * @brief Prints XCSF evolutionary algorithm parameters.
 * @param xcsf The XCSF data structure.
 */
static void
param_print_ea(const struct XCSF *xcsf)
{
    printf(", EA_SELECT_TYPE=%d", xcsf->EA_SELECT_TYPE);
    printf(", EA_SELECT_SIZE=%f", xcsf->EA_SELECT_SIZE);
    printf(", THETA_EA=%f", xcsf->THETA_EA);
    printf(", LAMBDA=%d", xcsf->LAMBDA);
    printf(", P_CROSSOVER=%f", xcsf->P_CROSSOVER);
}

/**
 * @brief Saves XCSF evolutionary algorithm parameters.
 * @param xcsf The XCSF data structure.
 * @param fp Pointer to the output file.
 * @return The total number of elements written.
 */
static size_t
param_save_ea(const struct XCSF *xcsf, FILE *fp)
{
    size_t s = 0;
    s += fwrite(&xcsf->EA_SELECT_TYPE, sizeof(int), 1, fp);
    s += fwrite(&xcsf->EA_SELECT_SIZE, sizeof(double), 1, fp);
    s += fwrite(&xcsf->THETA_EA, sizeof(double), 1, fp);
    s += fwrite(&xcsf->LAMBDA, sizeof(int), 1, fp);
    s += fwrite(&xcsf->P_CROSSOVER, sizeof(double), 1, fp);
    return s;
}

/**
 * @brief Loads XCSF evolutionary algorithm parameters.
 * @param xcsf The XCSF data structure.
 * @param fp Pointer to the output file.
 * @return The total number of elements written.
 */
static size_t
param_load_ea(struct XCSF *xcsf, FILE *fp)
{
    size_t s = 0;
    s += fread(&xcsf->EA_SELECT_TYPE, sizeof(int), 1, fp);
    s += fread(&xcsf->EA_SELECT_SIZE, sizeof(double), 1, fp);
    s += fread(&xcsf->THETA_EA, sizeof(double), 1, fp);
    s += fread(&xcsf->LAMBDA, sizeof(int), 1, fp);
    s += fread(&xcsf->P_CROSSOVER, sizeof(double), 1, fp);
    return s;
}

/**
 * @brief Initialises default classifier condition parameters.
 * @param xcsf The XCSF data structure.
 */
static void
param_defaults_cl_condition(struct XCSF *xcsf)
{
    param_set_cond_eta(xcsf, 0);
    param_set_cond_type(xcsf, 1);
    param_set_cond_min(xcsf, 0);
    param_set_cond_max(xcsf, 1);
    param_set_cond_smin(xcsf, 0.1);
    param_set_cond_bits(xcsf, 1);
    param_set_gp_num_cons(xcsf, 100);
    param_set_gp_init_depth(xcsf, 5);
    param_set_max_k(xcsf, 2);
    param_set_max_t(xcsf, 10);
    param_set_max_neuron_grow(xcsf, 1);
    param_set_cond_evolve_weights(xcsf, true);
    param_set_cond_evolve_neurons(xcsf, true);
    param_set_cond_evolve_functions(xcsf, false);
    param_set_cond_evolve_connectivity(xcsf, false);
    memset(xcsf->COND_NUM_NEURONS, 0, sizeof(int) * MAX_LAYERS);
    memset(xcsf->COND_MAX_NEURONS, 0, sizeof(int) * MAX_LAYERS);
    xcsf->COND_NUM_NEURONS[0] = 1;
    xcsf->COND_MAX_NEURONS[0] = 10;
    param_set_cond_output_activation(xcsf, 0);
    param_set_cond_hidden_activation(xcsf, 0);
}

/**
 * @brief Prints XCSF condtion parameters.
 * @param xcsf The XCSF data structure.
 */
static void
param_print_cl_condition(const struct XCSF *xcsf)
{
    printf(", COND_ETA=%f", xcsf->COND_ETA);
    printf(", COND_TYPE=%d", xcsf->COND_TYPE);
    printf(", COND_MIN=%f", xcsf->COND_MIN);
    printf(", COND_MAX=%f", xcsf->COND_MAX);
    printf(", COND_SMIN=%f", xcsf->COND_SMIN);
    printf(", COND_BITS=%d", xcsf->COND_BITS);
    printf(", GP_NUM_CONS=%d", xcsf->GP_NUM_CONS);
    printf(", GP_INIT_DEPTH=%d", xcsf->GP_INIT_DEPTH);
    printf(", MAX_K=%d", xcsf->MAX_K);
    printf(", MAX_T=%d", xcsf->MAX_T);
    printf(", MAX_NEURON_GROW=%d", xcsf->MAX_NEURON_GROW);
    printf(", COND_EVOLVE_WEIGHTS=");
    xcsf->COND_EVOLVE_WEIGHTS ? printf("true") : printf("false");
    printf(", COND_EVOLVE_NEURONS=");
    xcsf->COND_EVOLVE_NEURONS ? printf("true") : printf("false");
    printf(", COND_EVOLVE_FUNCTIONS=");
    xcsf->COND_EVOLVE_FUNCTIONS ? printf("true") : printf("false");
    printf(", COND_EVOLVE_CONNECTIVITY=");
    xcsf->COND_EVOLVE_CONNECTIVITY ? printf("true") : printf("false");
    printf(", COND_NUM_NEURONS=[");
    for (int i = 0; i < MAX_LAYERS && xcsf->COND_NUM_NEURONS[i] > 0; ++i) {
        printf("%d;", xcsf->COND_NUM_NEURONS[i]);
    }
    printf("]");
    printf(", COND_MAX_NEURONS=[");
    for (int i = 0; i < MAX_LAYERS && xcsf->COND_MAX_NEURONS[i] > 0; ++i) {
        printf("%d;", xcsf->COND_MAX_NEURONS[i]);
    }
    printf("]");
    printf(", COND_OUTPUT_ACTIVATION=%d", xcsf->COND_OUTPUT_ACTIVATION);
    printf(", COND_HIDDEN_ACTIVATION=%d", xcsf->COND_HIDDEN_ACTIVATION);
}

/**
 * @brief Saves XCSF classifier condition parameters.
 * @param xcsf The XCSF data structure.
 * @param fp Pointer to the output file.
 * @return The total number of elements written.
 */
static size_t
param_save_cl_condition(const struct XCSF *xcsf, FILE *fp)
{
    size_t s = 0;
    s += fwrite(&xcsf->COND_ETA, sizeof(double), 1, fp);
    s += fwrite(&xcsf->COND_TYPE, sizeof(int), 1, fp);
    s += fwrite(&xcsf->COND_MIN, sizeof(double), 1, fp);
    s += fwrite(&xcsf->COND_MAX, sizeof(double), 1, fp);
    s += fwrite(&xcsf->COND_SMIN, sizeof(double), 1, fp);
    s += fwrite(&xcsf->COND_BITS, sizeof(int), 1, fp);
    s += fwrite(&xcsf->GP_NUM_CONS, sizeof(int), 1, fp);
    s += fwrite(&xcsf->GP_INIT_DEPTH, sizeof(int), 1, fp);
    s += fwrite(xcsf->gp_cons, sizeof(double), xcsf->GP_NUM_CONS, fp);
    s += fwrite(&xcsf->MAX_K, sizeof(int), 1, fp);
    s += fwrite(&xcsf->MAX_T, sizeof(int), 1, fp);
    s += fwrite(&xcsf->MAX_NEURON_GROW, sizeof(int), 1, fp);
    s += fwrite(&xcsf->COND_EVOLVE_WEIGHTS, sizeof(_Bool), 1, fp);
    s += fwrite(&xcsf->COND_EVOLVE_NEURONS, sizeof(_Bool), 1, fp);
    s += fwrite(&xcsf->COND_EVOLVE_FUNCTIONS, sizeof(_Bool), 1, fp);
    s += fwrite(&xcsf->COND_EVOLVE_CONNECTIVITY, sizeof(_Bool), 1, fp);
    s += fwrite(xcsf->COND_NUM_NEURONS, sizeof(int), MAX_LAYERS, fp);
    s += fwrite(xcsf->COND_MAX_NEURONS, sizeof(int), MAX_LAYERS, fp);
    s += fwrite(&xcsf->COND_OUTPUT_ACTIVATION, sizeof(int), 1, fp);
    s += fwrite(&xcsf->COND_HIDDEN_ACTIVATION, sizeof(int), 1, fp);
    return s;
}

/**
 * @brief Loads XCSF classifier condition parameters.
 * @param xcsf The XCSF data structure.
 * @param fp Pointer to the output file.
 * @return The total number of elements written.
 */
static size_t
param_load_cl_condition(struct XCSF *xcsf, FILE *fp)
{
    size_t s = 0;
    s += fread(&xcsf->COND_ETA, sizeof(double), 1, fp);
    s += fread(&xcsf->COND_TYPE, sizeof(int), 1, fp);
    s += fread(&xcsf->COND_MIN, sizeof(double), 1, fp);
    s += fread(&xcsf->COND_MAX, sizeof(double), 1, fp);
    s += fread(&xcsf->COND_SMIN, sizeof(double), 1, fp);
    s += fread(&xcsf->COND_BITS, sizeof(int), 1, fp);
    s += fread(&xcsf->GP_NUM_CONS, sizeof(int), 1, fp);
    s += fread(&xcsf->GP_INIT_DEPTH, sizeof(int), 1, fp);
    if (xcsf->GP_NUM_CONS < 1) {
        printf("param_load_cl_condition(): read error\n");
        xcsf->GP_NUM_CONS = 1;
        exit(EXIT_FAILURE);
    }
    free(xcsf->gp_cons); // always allocated on start
    xcsf->gp_cons = malloc(sizeof(double) * xcsf->GP_NUM_CONS);
    s += fread(xcsf->gp_cons, sizeof(double), xcsf->GP_NUM_CONS, fp);
    s += fread(&xcsf->MAX_K, sizeof(int), 1, fp);
    s += fread(&xcsf->MAX_T, sizeof(int), 1, fp);
    s += fread(&xcsf->MAX_NEURON_GROW, sizeof(int), 1, fp);
    s += fread(&xcsf->COND_EVOLVE_WEIGHTS, sizeof(_Bool), 1, fp);
    s += fread(&xcsf->COND_EVOLVE_NEURONS, sizeof(_Bool), 1, fp);
    s += fread(&xcsf->COND_EVOLVE_FUNCTIONS, sizeof(_Bool), 1, fp);
    s += fread(&xcsf->COND_EVOLVE_CONNECTIVITY, sizeof(_Bool), 1, fp);
    s += fread(xcsf->COND_NUM_NEURONS, sizeof(int), MAX_LAYERS, fp);
    s += fread(xcsf->COND_MAX_NEURONS, sizeof(int), MAX_LAYERS, fp);
    s += fread(&xcsf->COND_OUTPUT_ACTIVATION, sizeof(int), 1, fp);
    s += fread(&xcsf->COND_HIDDEN_ACTIVATION, sizeof(int), 1, fp);
    return s;
}

/**
 * @brief Initialises default classifier prediction parameters.
 * @param xcsf The XCSF data structure.
 */
static void
param_defaults_cl_prediction(struct XCSF *xcsf)
{
    param_set_pred_type(xcsf, 1);
    param_set_pred_evolve_eta(xcsf, true);
    param_set_pred_eta(xcsf, 0.1);
    param_set_pred_reset(xcsf, false);
    param_set_pred_x0(xcsf, 1);
    param_set_pred_rls_scale_factor(xcsf, 1000);
    param_set_pred_rls_lambda(xcsf, 1);
    param_set_pred_evolve_weights(xcsf, true);
    param_set_pred_evolve_neurons(xcsf, true);
    param_set_pred_evolve_functions(xcsf, false);
    param_set_pred_evolve_connectivity(xcsf, false);
    param_set_pred_sgd_weights(xcsf, true);
    param_set_pred_momentum(xcsf, 0.9);
    param_set_pred_decay(xcsf, 0);
    memset(xcsf->PRED_NUM_NEURONS, 0, sizeof(int) * MAX_LAYERS);
    memset(xcsf->PRED_MAX_NEURONS, 0, sizeof(int) * MAX_LAYERS);
    xcsf->PRED_NUM_NEURONS[0] = 1;
    xcsf->PRED_MAX_NEURONS[0] = 10;
    param_set_pred_output_activation(xcsf, 0);
    param_set_pred_hidden_activation(xcsf, 0);
}

/**
 * @brief Prints XCSF prediction parameters.
 * @param xcsf The XCSF data structure.
 */
static void
param_print_cl_prediction(const struct XCSF *xcsf)
{
    printf(", PRED_TYPE=%d", xcsf->PRED_TYPE);
    printf(", PRED_EVOLVE_ETA=");
    xcsf->PRED_EVOLVE_ETA ? printf("true") : printf("false");
    printf(", PRED_ETA=%f", xcsf->PRED_ETA);
    printf(", PRED_RESET=");
    xcsf->PRED_RESET ? printf("true") : printf("false");
    printf(", PRED_X0=%f", xcsf->PRED_X0);
    printf(", PRED_RLS_SCALE_FACTOR=%f", xcsf->PRED_RLS_SCALE_FACTOR);
    printf(", PRED_RLS_LAMBDA=%f", xcsf->PRED_RLS_LAMBDA);
    printf(", PRED_EVOLVE_WEIGHTS=");
    xcsf->PRED_EVOLVE_WEIGHTS ? printf("true") : printf("false");
    printf(", PRED_EVOLVE_NEURONS=");
    xcsf->PRED_EVOLVE_NEURONS ? printf("true") : printf("false");
    printf(", PRED_EVOLVE_FUNCTIONS=");
    xcsf->PRED_EVOLVE_FUNCTIONS ? printf("true") : printf("false");
    printf(", PRED_EVOLVE_CONNECTIVITY=");
    xcsf->PRED_EVOLVE_CONNECTIVITY ? printf("true") : printf("false");
    printf(", PRED_SGD_WEIGHTS=");
    xcsf->PRED_SGD_WEIGHTS ? printf("true") : printf("false");
    printf(", PRED_MOMENTUM=%f", xcsf->PRED_MOMENTUM);
    printf(", PRED_DECAY=%f", xcsf->PRED_DECAY);
    printf(", PRED_NUM_NEURONS=[");
    for (int i = 0; i < MAX_LAYERS && xcsf->PRED_NUM_NEURONS[i] > 0; ++i) {
        printf("%d;", xcsf->PRED_NUM_NEURONS[i]);
    }
    printf("]");
    printf(", PRED_MAX_NEURONS=[");
    for (int i = 0; i < MAX_LAYERS && xcsf->PRED_MAX_NEURONS[i] > 0; ++i) {
        printf("%d;", xcsf->PRED_MAX_NEURONS[i]);
    }
    printf("]");
    printf(", PRED_OUTPUT_ACTIVATION=%d", xcsf->PRED_OUTPUT_ACTIVATION);
    printf(", PRED_HIDDEN_ACTIVATION=%d", xcsf->PRED_HIDDEN_ACTIVATION);
}

/**
 * @brief Saves XCSF classifier prediction parameters.
 * @param xcsf The XCSF data structure.
 * @param fp Pointer to the output file.
 * @return The total number of elements written.
 */
static size_t
param_save_cl_prediction(const struct XCSF *xcsf, FILE *fp)
{
    size_t s = 0;
    s += fwrite(&xcsf->PRED_TYPE, sizeof(int), 1, fp);
    s += fwrite(&xcsf->PRED_EVOLVE_ETA, sizeof(_Bool), 1, fp);
    s += fwrite(&xcsf->PRED_ETA, sizeof(double), 1, fp);
    s += fwrite(&xcsf->PRED_RESET, sizeof(_Bool), 1, fp);
    s += fwrite(&xcsf->PRED_X0, sizeof(double), 1, fp);
    s += fwrite(&xcsf->PRED_RLS_SCALE_FACTOR, sizeof(double), 1, fp);
    s += fwrite(&xcsf->PRED_RLS_LAMBDA, sizeof(double), 1, fp);
    s += fwrite(&xcsf->PRED_EVOLVE_WEIGHTS, sizeof(_Bool), 1, fp);
    s += fwrite(&xcsf->PRED_EVOLVE_NEURONS, sizeof(_Bool), 1, fp);
    s += fwrite(&xcsf->PRED_EVOLVE_FUNCTIONS, sizeof(_Bool), 1, fp);
    s += fwrite(&xcsf->PRED_EVOLVE_CONNECTIVITY, sizeof(_Bool), 1, fp);
    s += fwrite(&xcsf->PRED_SGD_WEIGHTS, sizeof(_Bool), 1, fp);
    s += fwrite(&xcsf->PRED_MOMENTUM, sizeof(double), 1, fp);
    s += fwrite(&xcsf->PRED_DECAY, sizeof(double), 1, fp);
    s += fwrite(xcsf->PRED_NUM_NEURONS, sizeof(int), MAX_LAYERS, fp);
    s += fwrite(xcsf->PRED_MAX_NEURONS, sizeof(int), MAX_LAYERS, fp);
    s += fwrite(&xcsf->PRED_OUTPUT_ACTIVATION, sizeof(int), 1, fp);
    s += fwrite(&xcsf->PRED_HIDDEN_ACTIVATION, sizeof(int), 1, fp);
    return s;
}

/**
 * @brief Loads XCSF classifier prediction parameters.
 * @param xcsf The XCSF data structure.
 * @param fp Pointer to the output file.
 * @return The total number of elements written.
 */
static size_t
param_load_cl_prediction(struct XCSF *xcsf, FILE *fp)
{
    size_t s = 0;
    s += fread(&xcsf->PRED_TYPE, sizeof(int), 1, fp);
    s += fread(&xcsf->PRED_EVOLVE_ETA, sizeof(_Bool), 1, fp);
    s += fread(&xcsf->PRED_ETA, sizeof(double), 1, fp);
    s += fread(&xcsf->PRED_RESET, sizeof(_Bool), 1, fp);
    s += fread(&xcsf->PRED_X0, sizeof(double), 1, fp);
    s += fread(&xcsf->PRED_RLS_SCALE_FACTOR, sizeof(double), 1, fp);
    s += fread(&xcsf->PRED_RLS_LAMBDA, sizeof(double), 1, fp);
    s += fread(&xcsf->PRED_EVOLVE_WEIGHTS, sizeof(_Bool), 1, fp);
    s += fread(&xcsf->PRED_EVOLVE_NEURONS, sizeof(_Bool), 1, fp);
    s += fread(&xcsf->PRED_EVOLVE_FUNCTIONS, sizeof(_Bool), 1, fp);
    s += fread(&xcsf->PRED_EVOLVE_CONNECTIVITY, sizeof(_Bool), 1, fp);
    s += fread(&xcsf->PRED_SGD_WEIGHTS, sizeof(_Bool), 1, fp);
    s += fread(&xcsf->PRED_MOMENTUM, sizeof(double), 1, fp);
    s += fread(&xcsf->PRED_DECAY, sizeof(double), 1, fp);
    s += fread(xcsf->PRED_NUM_NEURONS, sizeof(int), MAX_LAYERS, fp);
    s += fread(xcsf->PRED_MAX_NEURONS, sizeof(int), MAX_LAYERS, fp);
    s += fread(&xcsf->PRED_OUTPUT_ACTIVATION, sizeof(int), 1, fp);
    s += fread(&xcsf->PRED_HIDDEN_ACTIVATION, sizeof(int), 1, fp);
    return s;
}

/**
 * @brief Initialises default classifier action parameters.
 * @param xcsf The XCSF data structure.
 */
static void
param_defaults_cl_action(struct XCSF *xcsf)
{
    param_set_act_type(xcsf, 0);
}

/**
 * @brief Prints XCSF action parameters.
 * @param xcsf The XCSF data structure.
 */
static void
param_print_cl_action(const struct XCSF *xcsf)
{
    printf(", ACT_TYPE=%d", xcsf->ACT_TYPE);
}

/**
 * @brief Saves XCSF classifier action parameters.
 * @param xcsf The XCSF data structure.
 * @param fp Pointer to the output file.
 * @return The total number of elements written.
 */
static size_t
param_save_cl_action(const struct XCSF *xcsf, FILE *fp)
{
    size_t s = 0;
    s += fwrite(&xcsf->ACT_TYPE, sizeof(int), 1, fp);
    return s;
}

/**
 * @brief Loads XCSF classifier action parameters.
 * @param xcsf The XCSF data structure.
 * @param fp Pointer to the output file.
 * @return The total number of elements written.
 */
static size_t
param_load_cl_action(struct XCSF *xcsf, FILE *fp)
{
    size_t s = 0;
    s += fread(&xcsf->ACT_TYPE, sizeof(int), 1, fp);
    return s;
}

/**
 * @brief Initialises default XCSF parameters.
 * @param xcsf The XCSF data structure.
 */
void
param_init(struct XCSF *xcsf)
{
    xcsf->gp_cons = NULL;
    xcsf->time = 0;
    xcsf->error = xcsf->EPS_0;
    xcsf->msetsize = 0;
    xcsf->mfrac = 0;
    param_defaults_cl_action(xcsf);
    param_defaults_cl_condition(xcsf);
    param_defaults_cl_general(xcsf);
    param_defaults_cl_prediction(xcsf);
    param_defaults_ea(xcsf);
    param_defaults_general(xcsf);
    param_defaults_multistep(xcsf);
    param_defaults_subsumption(xcsf);
}

/**
 * @brief Frees XCSF parameter memory.
 * @param xcsf The XCSF data structure.
 */
void
param_free(const struct XCSF *xcsf)
{
    tree_free_cons(xcsf);
}

/**
 * @brief Prints all XCSF parameters.
 * @param xcsf The XCSF data structure.
 */
void
param_print(const struct XCSF *xcsf)
{
    printf("VERSION=%d.%d.%d, ", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);
    param_print_general(xcsf);
    param_print_multistep(xcsf);
    param_print_ea(xcsf);
    param_print_subsumption(xcsf);
    param_print_cl_general(xcsf);
    param_print_cl_condition(xcsf);
    param_print_cl_prediction(xcsf);
    param_print_cl_action(xcsf);
    printf("\n");
}

/**
 * @brief Writes the XCSF data structure to a binary file.
 * @param xcsf The XCSF data structure.
 * @param fp Pointer to the output file.
 * @return The total number of elements written.
 */
size_t
param_save(const struct XCSF *xcsf, FILE *fp)
{
    size_t s = 0;
    s += fwrite(&xcsf->time, sizeof(int), 1, fp);
    s += fwrite(&xcsf->error, sizeof(double), 1, fp);
    s += fwrite(&xcsf->msetsize, sizeof(double), 1, fp);
    s += fwrite(&xcsf->mfrac, sizeof(double), 1, fp);
    s += fwrite(&xcsf->explore, sizeof(_Bool), 1, fp);
    s += fwrite(&xcsf->x_dim, sizeof(int), 1, fp);
    s += fwrite(&xcsf->y_dim, sizeof(int), 1, fp);
    s += fwrite(&xcsf->n_actions, sizeof(int), 1, fp);
    s += param_save_general(xcsf, fp);
    s += param_save_multistep(xcsf, fp);
    s += param_save_ea(xcsf, fp);
    s += param_save_subsumption(xcsf, fp);
    s += param_save_cl_general(xcsf, fp);
    s += param_save_cl_condition(xcsf, fp);
    s += param_save_cl_prediction(xcsf, fp);
    s += param_save_cl_action(xcsf, fp);
    return s;
}

/**
 * @brief Reads the XCSF data structure from a binary file.
 * @param xcsf The XCSF data structure.
 * @param fp Pointer to the input file.
 * @return The total number of elements read.
 */
size_t
param_load(struct XCSF *xcsf, FILE *fp)
{
    size_t s = 0;
    s += fread(&xcsf->time, sizeof(int), 1, fp);
    s += fread(&xcsf->error, sizeof(double), 1, fp);
    s += fread(&xcsf->msetsize, sizeof(double), 1, fp);
    s += fread(&xcsf->mfrac, sizeof(double), 1, fp);
    s += fread(&xcsf->explore, sizeof(_Bool), 1, fp);
    s += fread(&xcsf->x_dim, sizeof(int), 1, fp);
    s += fread(&xcsf->y_dim, sizeof(int), 1, fp);
    s += fread(&xcsf->n_actions, sizeof(int), 1, fp);
    if (xcsf->x_dim < 1 || xcsf->y_dim < 1) {
        printf("param_load(): read error\n");
        exit(EXIT_FAILURE);
    }
    s += param_load_general(xcsf, fp);
    s += param_load_multistep(xcsf, fp);
    s += param_load_ea(xcsf, fp);
    s += param_load_subsumption(xcsf, fp);
    s += param_load_cl_general(xcsf, fp);
    s += param_load_cl_condition(xcsf, fp);
    s += param_load_cl_prediction(xcsf, fp);
    s += param_load_cl_action(xcsf, fp);
    return s;
}

/**
 * @brief Sets the number of OMP threads.
 * @param xcsf The XCSF data structure.
 * @param a The number of threads.
 */
void
param_set_omp_num_threads(struct XCSF *xcsf, const int a)
{
    if (a < 1) {
        printf("Warning: tried to set OMP_NUM_THREADS too small\n");
        xcsf->OMP_NUM_THREADS = 1;
    } else if (a > 1000) {
        printf("Warning: tried to set OMP_NUM_THREADS too large\n");
        xcsf->OMP_NUM_THREADS = 1000;
    } else {
        xcsf->OMP_NUM_THREADS = a;
    }
#ifdef PARALLEL
    omp_set_num_threads(xcsf->OMP_NUM_THREADS);
#endif
}
