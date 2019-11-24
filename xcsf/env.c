/*
 * Copyright (C) 2015--2019 Richard Preen <rpreen@gmail.com>
 *
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
 * @file env.c
 * @brief Built-in problem environment interface.
 */ 
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "xcsf.h"
#include "utils.h"
#include "env.h"
#include "env_mux.h"
#include "env_maze.h"

void env_init(XCSF *xcsf, char **argv)
{
    if(strcmp(argv[1], "mp") == 0) {
        xcsf->env_vptr = &env_mux_vtbl;
        env_mux_init(xcsf, atoi(argv[2]));
    }
    else if(strcmp(argv[1], "maze") == 0) {
        xcsf->env_vptr = &env_maze_vtbl;
        env_maze_init(xcsf, argv[2]);
    }
    else {
        printf("Invalid environment specified: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }
}