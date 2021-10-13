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
 * @file cond_ub_rectangle.h
 * @author David Pätzel <david.paetzel@posteo.de>
 * @copyright The Authors.
 * @date 2019--2020.
 * @brief Unordered bound hyperrectangle condition functions.
 */

#pragma once

#include "condition.h"
#include "xcsf.h"

/**
 * @brief Unordered bound hyperrectangle condition data structure.
 */
struct CondUBRectangle {
    double *lower; //!< Lower bound
    double *upper; //!< Upper bound
    double *mu; //!< Mutation rates
};

bool
cond_ub_rectangle_reorder(const struct XCSF *xcsf, const struct CondUBRectangle *cond);

bool
cond_ub_rectangle_crossover(const struct XCSF *xcsf, const struct Cl *c1,
                         const struct Cl *c2);

bool
cond_ub_rectangle_general(const struct XCSF *xcsf, const struct Cl *c1,
                       const struct Cl *c2);

bool
cond_ub_rectangle_match(const struct XCSF *xcsf, const struct Cl *c,
                     const double *x);

bool
cond_ub_rectangle_mutate(const struct XCSF *xcsf, const struct Cl *c);

void
cond_ub_rectangle_copy(const struct XCSF *xcsf, struct Cl *dest,
                    const struct Cl *src);

void
cond_ub_rectangle_cover(const struct XCSF *xcsf, const struct Cl *c,
                     const double *x);

void
cond_ub_rectangle_free(const struct XCSF *xcsf, const struct Cl *c);

void
cond_ub_rectangle_init(const struct XCSF *xcsf, struct Cl *c);

void
cond_ub_rectangle_print(const struct XCSF *xcsf, const struct Cl *c);

void
cond_ub_rectangle_update(const struct XCSF *xcsf, const struct Cl *c,
                      const double *x, const double *y);

double
cond_ub_rectangle_size(const struct XCSF *xcsf, const struct Cl *c);

size_t
cond_ub_rectangle_save(const struct XCSF *xcsf, const struct Cl *c, FILE *fp);

size_t
cond_ub_rectangle_load(const struct XCSF *xcsf, struct Cl *c, FILE *fp);

const char *
cond_ub_rectangle_json(const struct XCSF *xcsf, const struct Cl *c);

/**
 * @brief Hyperrectangle condition implemented functions.
 */
static struct CondVtbl const cond_ub_rectangle_vtbl = {
    &cond_ub_rectangle_crossover, &cond_ub_rectangle_general, &cond_ub_rectangle_match,
    &cond_ub_rectangle_mutate,    &cond_ub_rectangle_copy,    &cond_ub_rectangle_cover,
    &cond_ub_rectangle_free,      &cond_ub_rectangle_init,    &cond_ub_rectangle_print,
    &cond_ub_rectangle_update,    &cond_ub_rectangle_size,    &cond_ub_rectangle_save,
    &cond_ub_rectangle_load,      &cond_ub_rectangle_json
};
