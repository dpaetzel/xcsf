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
 * @file cond_ub_rectangle.c
 * @author David Pätzel <david.paetzel@posteo.de>
 * @copyright The Authors.
 * @date 2019--2020.
 * @brief Unordered bound hyperrectangle condition functions.
 */

#include "cond_ub_rectangle.h"
#include "ea.h"
#include "sam.h"
#include "utils.h"

#define N_MU (1) //!< Number of hyperrectangle mutation rates

/**
 * @brief Self-adaptation method for mutating hyperrectangles.
 */
static const int MU_TYPE[N_MU] = { SAM_LOG_NORMAL };

/**
 * @brief Reorders and resizes an unordered hyperrectangle condition.
 * @details Resizes to minimum spread if spread is smaller than minimum spread.
 * @param [in] xcsf XCSF data structure.
 * @param [in] cond Condition to reorder.
 * @return Whether any alterations were made.
 */
bool
cond_ub_rectangle_reorder_resize(const struct XCSF *xcsf, const struct CondUBRectangle *cond)
{
    bool changed = false;

    for (int i = 0; i < xcsf->x_dim; ++i) {
        if (cond->lower[i] > cond->upper[i]) {
            cond->lower[i] = fmin(cond->lower[i], cond->upper[i]);
            cond->upper[i] = fmax(cond->lower[i], cond->upper[i]);
            changed = true;
        }
        const double spread_i = (cond->upper[i] - cond->lower[i]) / 2;
        if (spread_i < xcsf->cond->spread_min) {
            cond->upper[i] += xcsf->cond->spread_min - spread_i;
            cond->lower[i] -= xcsf->cond->spread_min - spread_i;
            changed = true;
        }
    }

    return changed;
}

/**
 * @brief Returns the relative distance to a hyperrectangle.
 * @details Distance is zero at the center; one on the border; and greater than
 * one outside of the hyperrectangle.
 * @param [in] xcsf XCSF data structure.
 * @param [in] c Classifier whose hyperrectangle distance is to be computed.
 * @param [in] x Input to compute the relative distance.
 * @return The relative distance of an input to the hyperrectangle.
 */
static double
cond_ub_rectangle_dist(const struct XCSF *xcsf, const struct Cl *c,
                       const double *x)
{
    const struct CondUBRectangle *cond = c->cond;
    double dist = 0;
    for (int i = 0; i < xcsf->x_dim; ++i) {
        // TODO Make this more efficient
        const double spread_i = (cond->upper[i] - cond->lower[i]) / 2;
        const double d = fabs((x[i] - cond->lower[i] - spread_i) / spread_i);
        if (d > dist) {
            dist = d;
        }
    }
    return dist;
}

/**
 * @brief Creates and initialises a hyperrectangle condition.
 * @details Uses the unordered bound representation.
 * @param [in] xcsf XCSF data structure.
 * @param [in] c Classifier whose condition is to be initialised.
 */
void
cond_ub_rectangle_init(const struct XCSF *xcsf, struct Cl *c)
{
    struct CondUBRectangle *new = malloc(sizeof(struct CondUBRectangle));
    new->lower = malloc(sizeof(double) * xcsf->x_dim);
    new->upper = malloc(sizeof(double) * xcsf->x_dim);
    for (int i = 0; i < xcsf->x_dim; ++i) {
        new->lower[i] = rand_uniform(xcsf->cond->min, xcsf->cond->max);
        new->upper[i] = rand_uniform(xcsf->cond->min, xcsf->cond->max);
    }
    cond_ub_rectangle_reorder_resize(xcsf, new);
    new->mu = malloc(sizeof(double) * N_MU);
    sam_init(new->mu, N_MU, MU_TYPE);
    c->cond = new;
}

/**
 * @brief Frees the memory used by a hyperrectangle condition.
 * @param [in] xcsf XCSF data structure.
 * @param [in] c Classifier whose condition is to be freed.
 */
void
cond_ub_rectangle_free(const struct XCSF *xcsf, const struct Cl *c)
{
    (void) xcsf;
    const struct CondUBRectangle *cond = c->cond;
    free(cond->lower);
    free(cond->upper);
    free(cond->mu);
    free(c->cond);
}

/**
 * @brief Copies a hyperrectangle condition from one classifier to another.
 * @param [in] xcsf XCSF data structure.
 * @param [in] dest Destination classifier.
 * @param [in] src Source classifier.
 */
void
cond_ub_rectangle_copy(const struct XCSF *xcsf, struct Cl *dest,
                       const struct Cl *src)
{
    struct CondUBRectangle *new = malloc(sizeof(struct CondUBRectangle));
    const struct CondUBRectangle *src_cond = src->cond;
    new->lower = malloc(sizeof(double) * xcsf->x_dim);
    new->upper = malloc(sizeof(double) * xcsf->x_dim);
    new->mu = malloc(sizeof(double) * N_MU);
    memcpy(new->lower, src_cond->lower, sizeof(double) * xcsf->x_dim);
    memcpy(new->upper, src_cond->upper, sizeof(double) * xcsf->x_dim);
    memcpy(new->mu, src_cond->mu, sizeof(double) * N_MU);
    dest->cond = new;
}

/**
 * @brief Generates a hyperrectangle that matches the current input.
 * @param [in] xcsf XCSF data structure.
 * @param [in] c Classifier whose condition is being covered.
 * @param [in] x Input state to cover.
 */
void
cond_ub_rectangle_cover(const struct XCSF *xcsf, const struct Cl *c,
                        const double *x)
{
    const struct CondUBRectangle *cond = c->cond;
    const double spread_max = fabs(xcsf->cond->max - xcsf->cond->min);
    for (int i = 0; i < xcsf->x_dim; ++i) {
        // NOTE We center on the current observation but we could also generate
        // two random numbers instead of one.
        const double spread_i = rand_uniform(xcsf->cond->spread_min, spread_max);
        cond->lower[i] = x[i] - spread_i;
        cond->upper[i] = x[i] + spread_i;
    }
}

/**
 * @brief Updates a hyperrectangle, sliding the centers towards the mean input.
 * @param [in] xcsf XCSF data structure.
 * @param [in] c Classifier whose condition is to be updated.
 * @param [in] x Input state.
 * @param [in] y Truth/payoff value.
 */
void
cond_ub_rectangle_update(const struct XCSF *xcsf, const struct Cl *c,
                         const double *x, const double *y)
{
    (void) y;
    if (xcsf->cond->eta > 0) {
        const struct CondUBRectangle *cond = c->cond;

        for (int i = 0; i < xcsf->x_dim; ++i) {
            const double center_i = (cond->lower[i] + cond->upper[i]) / 2;
            const double spread_i = (cond->upper[i] - cond->lower[i]) / 2;
            const double center_i_new = xcsf->cond->eta * (x[i] - center_i);
            cond->lower[i] = center_i_new - spread_i;
            cond->upper[i] = center_i_new + spread_i;
        }
    }
}

/**
 * @brief Calculates whether a hyperrectangle condition matches an input.
 * @param [in] xcsf XCSF data structure.
 * @param [in] c Classifier whose condition to match.
 * @param [in] x Input state.
 * @return Whether the hyperrectangle condition matches the input.
 */
bool
cond_ub_rectangle_match(const struct XCSF *xcsf, const struct Cl *c,
                        const double *x)
{
    return (cond_ub_rectangle_dist(xcsf, c, x) < 1);
}

/**
 * @brief Performs uniform crossover with two hyperrectangle conditions.
 * @param [in] xcsf XCSF data structure.
 * @param [in] c1 First classifier whose condition is being crossed.
 * @param [in] c2 Second classifier whose condition is being crossed.
 * @return Whether any alterations were made.
 */
bool
cond_ub_rectangle_crossover(const struct XCSF *xcsf, const struct Cl *c1,
                            const struct Cl *c2)
{
    const struct CondUBRectangle *cond1 = c1->cond;
    const struct CondUBRectangle *cond2 = c2->cond;
    bool changed = false;
    if (rand_uniform(0, 1) < xcsf->ea->p_crossover) {
        for (int i = 0; i < xcsf->x_dim; ++i) {
            if (rand_uniform(0, 1) < 0.5) {
                const double tmp = cond1->lower[i];
                cond1->lower[i] = cond2->lower[i];
                cond2->lower[i] = tmp;
                changed = true;
            }
            if (rand_uniform(0, 1) < 0.5) {
                const double tmp = cond1->upper[i];
                cond1->upper[i] = cond2->upper[i];
                cond2->upper[i] = tmp;
                changed = true;
            }
        }
    }
    if (changed) {
        cond_ub_rectangle_reorder_resize(xcsf, cond1);
        cond_ub_rectangle_reorder_resize(xcsf, cond2);
    }
    return changed;
}

/**
 * @brief Mutates a hyperrectangle condition with the self-adaptive rate.
 * @param [in] xcsf XCSF data structure.
 * @param [in] c Classifier whose condition is being mutated.
 * @return Whether any alterations were made.
 */
bool
cond_ub_rectangle_mutate(const struct XCSF *xcsf, const struct Cl *c)
{
    bool changed = false;
    const struct CondUBRectangle *cond = c->cond;
    double *lower = cond->lower;
    double *upper = cond->upper;
    sam_adapt(cond->mu, N_MU, MU_TYPE);
    for (int i = 0; i < xcsf->x_dim; ++i) {
        double orig = lower[i];
        lower[i] += rand_normal(0, cond->mu[0]);
        lower[i] = clamp(lower[i], xcsf->cond->min, xcsf->cond->max);
        if (orig != lower[i]) {
            changed = true;
        }
        orig = upper[i];
        upper[i] += rand_normal(0, cond->mu[0]);
        upper[i] = clamp(lower[i], xcsf->cond->min, xcsf->cond->max);
        if (orig != upper[i]) {
            changed = true;
        }
    }
    if (changed) {
        cond_ub_rectangle_reorder_resize(xcsf, cond);
    }
    return changed;
}

/**
 * @brief Returns whether classifier c1 has a condition more general than c2.
 * @param [in] xcsf XCSF data structure.
 * @param [in] c1 Classifier whose condition is tested to be more general.
 * @param [in] c2 Classifier whose condition is tested to be more specific.
 * @return Whether the hyperrectangle condition of c1 is more general than c2.
 */
bool
cond_ub_rectangle_general(const struct XCSF *xcsf, const struct Cl *c1,
                          const struct Cl *c2)
{
    const struct CondUBRectangle *cond1 = c1->cond;
    const struct CondUBRectangle *cond2 = c2->cond;
    for (int i = 0; i < xcsf->x_dim; ++i) {
        if (cond1->lower[i] > cond2->lower[i] || cond1->upper[i] < cond2->upper[i]) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Prints a hyperrectangle condition.
 * @param [in] xcsf XCSF data structure.
 * @param [in] c Classifier whose condition is to be printed.
 */
void
cond_ub_rectangle_print(const struct XCSF *xcsf, const struct Cl *c)
{
    const struct CondUBRectangle *cond = c->cond;
    printf("rectangle:");
    for (int i = 0; i < xcsf->x_dim; ++i) {
        printf(" (l=%5f, ", cond->lower[i]);
        printf("u=%5f)", cond->upper[i]);
    }
    printf("\n");
}

/**
 * @brief Returns the size of a hyperrectangle condition.
 * @param [in] xcsf XCSF data structure.
 * @param [in] c Classifier whose condition size to return.
 * @return The length of the input dimension.
 */
double
cond_ub_rectangle_size(const struct XCSF *xcsf, const struct Cl *c)
{
    (void) c;
    return xcsf->x_dim;
}

/**
 * @brief Writes a hyperrectangle condition to a file.
 * @param [in] xcsf XCSF data structure.
 * @param [in] c Classifier whose condition is to be written.
 * @param [in] fp Pointer to the file to be written.
 * @return The number of elements written.
 */
size_t
cond_ub_rectangle_save(const struct XCSF *xcsf, const struct Cl *c, FILE *fp)
{
    size_t s = 0;
    const struct CondUBRectangle *cond = c->cond;
    s += fwrite(cond->lower, sizeof(double), xcsf->x_dim, fp);
    s += fwrite(cond->upper, sizeof(double), xcsf->x_dim, fp);
    s += fwrite(cond->mu, sizeof(double), N_MU, fp);
    return s;
}

/**
 * @brief Reads a hyperrectangle condition from a file.
 * @param [in] xcsf XCSF data structure.
 * @param [in] c Classifier whose condition is to be read.
 * @param [in] fp Pointer to the file to be read.
 * @return The number of elements read.
 */
size_t
cond_ub_rectangle_load(const struct XCSF *xcsf, struct Cl *c, FILE *fp)
{
    size_t s = 0;
    struct CondUBRectangle *new = malloc(sizeof(struct CondUBRectangle));
    new->lower = malloc(sizeof(double) * xcsf->x_dim);
    new->upper = malloc(sizeof(double) * xcsf->x_dim);
    new->mu = malloc(sizeof(double) * N_MU);
    s += fread(new->lower, sizeof(double), xcsf->x_dim, fp);
    s += fread(new->upper, sizeof(double), xcsf->x_dim, fp);
    s += fread(new->mu, sizeof(double), N_MU, fp);
    c->cond = new;
    return s;
}

/**
 * @brief Returns a json formatted string representation of a hyperrectangle.
 * @param [in] xcsf XCSF data structure.
 * @param [in] c Classifier whose condition is to be returned.
 * @return String encoded in json format.
 */
const char *
cond_ub_rectangle_json(const struct XCSF *xcsf, const struct Cl *c)
{
    const struct CondUBRectangle *cond = c->cond;
    cJSON *json = cJSON_CreateObject();
    cJSON *type = cJSON_CreateString("ubhyperrectangle");
    cJSON_AddItemToObject(json, "type", type);
    cJSON *center = cJSON_CreateDoubleArray(cond->lower, xcsf->x_dim);
    cJSON *spread = cJSON_CreateDoubleArray(cond->upper, xcsf->x_dim);
    cJSON *mutation = cJSON_CreateDoubleArray(cond->mu, N_MU);
    cJSON_AddItemToObject(json, "lower", center);
    cJSON_AddItemToObject(json, "upper", spread);
    cJSON_AddItemToObject(json, "mutation", mutation);
    const char *string = cJSON_Print(json);
    cJSON_Delete(json);
    return string;
}
