/*
 * Copyright (C) 2016--2019 Richard Preen <rpreen@gmail.com>
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
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <float.h>
#include "xcsf.h"
#include "random.h"
#include "neural_activations.h"
#include "neural.h"
#include "neural_layer_connected.h"
 
void neural_layer_connected_print(LAYER *l, _Bool print_weights)
{
    printf("nin = %d, nout = %d, activ = %s, ", 
        l->num_inputs, l->num_outputs, activation_string(l->activation_type));
    printf("weights (%d): ", l->num_weights);
    if(print_weights) {
        for(int i = 0; i < l->num_weights; i++) {
            printf(" %.4f, ", l->weights[i]);
        }
    }
    printf("biases (%d): ", l->num_outputs);
    if(print_weights) {
        for(int i = 0; i < l->num_outputs; i++) {
            printf(" %.4f, ", l->biases[i]);
        }
    }
    printf("\n");
}

void neural_layer_connected_init(LAYER *l, int num_inputs, int num_outputs, int activation)
{
    l->num_inputs = num_inputs;
    l->num_outputs = num_outputs;
    l->num_weights = num_inputs*num_outputs;
    l->input = calloc(l->num_inputs, sizeof(double));
    l->state = calloc(l->num_outputs, sizeof(double));
    l->output = calloc(l->num_outputs, sizeof(double));
    l->weights = calloc(l->num_weights, sizeof(double));
    l->biases = calloc(l->num_outputs, sizeof(double));
    l->bias_updates = calloc(l->num_outputs, sizeof(double));
    l->weight_updates = calloc(l->num_weights, sizeof(double));
    l->delta = calloc(l->num_outputs, sizeof(double));
    l->activation_type = activation;
    activation_set(&l->activate, activation);
    gradient_set(&l->gradient, activation);
    for(int i = 0; i < l->num_weights; i++) {
        l->weights[i] = rand_uniform(-0.1,0.1);
    }
    for(int i = 0; i < l->num_outputs; i++) {
        l->biases[i] = rand_uniform(-0.1,0.1);
    }
    l->forward = &neural_layer_connected_forward;
    l->backward = &neural_layer_connected_backward;
    l->update = &neural_layer_connected_update;
}

void neural_layer_connected_copy(LAYER *to, LAYER *from)
{
    to->num_inputs = from->num_inputs;
    to->num_outputs = from->num_outputs;
    to->num_weights = from->num_weights;
    memcpy(to->weights, from->weights, from->num_weights*sizeof(double));
    memcpy(to->biases, from->biases, from->num_outputs*sizeof(double));
    to->activation_type = from->activation_type;
    to->activate = from->activate;
    to->gradient = from->gradient;
}

void neural_layer_connected_free(LAYER *l)
{
    free(l->input);
    free(l->state);
    free(l->output);
    free(l->weights);
    free(l->biases);
    free(l->bias_updates);
    free(l->weight_updates);
    free(l->delta);
}

void neural_layer_connected_rand(LAYER *l)
{
    for(int i = 0; i < l->num_weights; i++) {
        l->weights[i] = rand_uniform(-1,1);
    }
    for(int i = 0; i < l->num_outputs; i++) {
        l->biases[i] = rand_uniform(-1,1);
    }
}

void neural_layer_connected_forward(LAYER *l, double *input)
{
    // store input
    memcpy(l->input, input, l->num_inputs*sizeof(double));
    // propagate each neuron
    for(int i = 0; i < l->num_outputs; i++) {
        l->state[i] = 0.0;
        // weights
        for(int j = 0; j < l->num_inputs; j++) {
            l->state[i] += input[j] * l->weights[i*l->num_inputs+j];
        }
        // bias
        l->state[i] += l->biases[i];
        // output
        l->output[i] = (l->activate)(l->state[i]);
        l->output[i] = fmax(DBL_MIN, fmin(DBL_MAX, l->output[i]));
    }
}

void neural_layer_connected_backward(LAYER *l)
{
    // apply gradients
    for(int i = 0; i < l->num_outputs; i++) {
        l->delta[i] *= (l->gradient)(l->state[i]);
    }
    // calculate bias updates
    for(int i = 0; i < l->num_outputs; i++) {
        l->bias_updates[i] += l->delta[i];
    }
    // calculate weight updates
    for(int i = 0; i < l->num_outputs; i++) {
        for(int j = 0; j < l->num_inputs; j++) {
            l->weight_updates[i*l->num_inputs+j] += l->delta[i] * l->input[j];
        }
    }   
}

void neural_layer_connected_update(XCSF *xcsf, LAYER *l)
{
    for(int i = 0; i < l->num_outputs; i++) {
        l->biases[i] += xcsf->ETA * l->bias_updates[i];
        l->bias_updates[i] *= xcsf->MOMENTUM;
    }

    for(int i = 0; i < l->num_weights; i++) {
        l->weights[i] += xcsf->ETA * l->weight_updates[i];
        l->weight_updates[i] *= xcsf->MOMENTUM;
    }
}

_Bool neural_layer_connected_mutate(XCSF *xcsf, LAYER *l)
{
    _Bool mod = false;
    // mutate weights
    for(int i = 0; i < l->num_weights; i++) {
        if(rand_uniform(0,1) < xcsf->P_MUTATION) {
            double orig = l->weights[i];
            l->weights[i] += rand_uniform(-1,1) * xcsf->S_MUTATION;
            if(l->weights[i] != orig) {
                mod = true;
            }
        }
    }
    // mutate biases
    for(int i = 0; i < l->num_outputs; i++) {
        if(rand_uniform(0,1) < xcsf->P_MUTATION) {
            double orig = l->biases[i];
            l->biases[i] += rand_uniform(-1,1) * xcsf->S_MUTATION;
            if(l->biases[i] != orig) {
                mod = true;
            }
        }
    }
    // mutate activation function
    if(rand_uniform(0,1) < xcsf->P_FUNC_MUTATION) {
        int rand_activate = irand_uniform(0,NUM_ACTIVATIONS);
        activation_set(&l->activate, rand_activate);
        gradient_set(&l->gradient, rand_activate);
        mod = true;
    } 
    return mod;
}
