#pragma once

#include "oclmp.hpp"
#include <CL/cl.h>
#include <cstddef>

cl_event oclmp_enqueue_mul(
        oclmp_env env, 
        size_t count, 
        size_t a, size_t b, size_t c, 
        cl_mem A, cl_mem B, cl_mem C, 
        int num_events, cl_event **wait_list);

cl_event oclmp_enqueue_add(
        oclmp_env env, 
        size_t count, 
        size_t a, size_t b, size_t c, 
        cl_mem A, cl_mem B, cl_mem C, 
        int num_events, cl_event **wait_list);