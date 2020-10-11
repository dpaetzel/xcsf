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
 * @file action.c
 * @author Richard Preen <rpreen@gmail.com>
 * @copyright The Authors.
 * @date 2015--2020.
 * @brief Interface for classifier actions.
 */

#include "action.h"
#include "act_integer.h"
#include "act_neural.h"
#include "utils.h"

/**
 * @brief Sets a classifier's action functions to the implementations.
 * @param [in] xcsf The XCSF data structure.
 * @param [in] c The classifier to set.
 */
void
action_set(const struct XCSF *xcsf, struct Cl *c)
{
    switch (xcsf->ACT_TYPE) {
        case ACT_TYPE_INTEGER:
            c->act_vptr = &act_integer_vtbl;
            break;
        case ACT_TYPE_NEURAL:
            c->act_vptr = &act_neural_vtbl;
            break;
        default:
            printf("Invalid action type specified: %d\n", xcsf->ACT_TYPE);
            exit(EXIT_FAILURE);
    }
}

/**
 * @brief Returns a string representation of an action type from an integer.
 * @param [in] type Integer representation of an action type.
 * @return String representing the name of the action type.
 */
const char *
action_type_as_string(const int type)
{
    switch (type) {
        case ACT_TYPE_INTEGER:
            return ACT_STRING_INTEGER;
        case ACT_TYPE_NEURAL:
            return ACT_STRING_NEURAL;
        default:
            printf("action_type_as_string(): invalid type: %d\n", type);
            exit(EXIT_FAILURE);
    }
}

/**
 * @brief Returns the integer representation of an action type given a name.
 * @param [in] type String representation of a condition type.
 * @return Integer representing the action type.
 */
int
action_type_as_int(const char *type)
{
    if (strncmp(type, ACT_STRING_INTEGER, 8) == 0) {
        return ACT_TYPE_INTEGER;
    }
    if (strncmp(type, ACT_STRING_NEURAL, 7) == 0) {
        return ACT_TYPE_NEURAL;
    }
    printf("action_type_as_int(): invalid type: %s\n", type);
    exit(EXIT_FAILURE);
}
