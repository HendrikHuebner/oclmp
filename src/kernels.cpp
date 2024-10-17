#include "kernels.hpp"
#include "computation.hpp"
#include <iostream>
#include <stdexcept>
#include <string>

cl_event oclmp_enqueue_mul(
        oclmp_env env, 
        size_t count, 
        size_t a, size_t b, size_t c, 
        cl_mem A, cl_mem B, cl_mem C, 
        int num_events, cl_event **wait_list) {

    cl_kernel kernel = env.getKernel("mul", "oclmp_mul");
    cl_kernel kernel_combine = env.getKernel("mul", "oclmp_combine");

    if (c < b + a) {
        std::cout << "Warning: multiplication result might not fit in the provided oclmp type" << std::endl;
    }

    if (a != b) {
        std::cout << "Warning: multiplication with different operand sizes not implemented" << std::endl;
    }

    cl_int err;
    cl_mem t = clCreateBuffer(env.ocl.ctx, CL_MEM_READ_WRITE, c * count, nullptr, &err);
    if (err != CL_SUCCESS) {
        throw std::runtime_error("Failed to create oclmp buffer on GPU: Error " + std::to_string(err));
    }

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &A);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &B);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &C);
    err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &t);

    if (err != CL_SUCCESS) {
        throw std::runtime_error("Failed to set kernel arguments: Error: " + std::to_string(err));
    }

    size_t global_work_size = a / 2;

    cl_event wait_events[num_events];
    for (int i = 0; i < num_events; i++) {
        wait_events[i] = *wait_list[i];
    }

    cl_event e1;
    err = clEnqueueNDRangeKernel(env.ocl.queue, kernel, 1, nullptr, &global_work_size, nullptr, num_events, wait_events, &e1);
    
    if (err != CL_SUCCESS) {
        throw std::runtime_error("Failed to enqueue kernel: Error " + std::to_string(err));
    }

    int a_s = global_work_size;
    err |= clSetKernelArg(kernel_combine, 0, sizeof(int), &a_s);
    err |= clSetKernelArg(kernel_combine, 1, sizeof(cl_mem), &B);
    err |= clSetKernelArg(kernel_combine, 2, sizeof(cl_mem), &t);
    err |= clSetKernelArg(kernel_combine, 3, sizeof(cl_mem), &C);

    if (err != CL_SUCCESS) {
        throw std::runtime_error("Failed to set kernel_combine arguments: Error: " + std::to_string(err));
    }

    global_work_size = 1;
    cl_event e2;
    err = clEnqueueNDRangeKernel(env.ocl.queue, kernel_combine, 1, nullptr, &global_work_size, nullptr, 1, &e1, &e2);
    if (err != CL_SUCCESS) {
        throw std::runtime_error("Failed to execute kernel_combine: Error " + std::to_string(err));
    }

    return e2;
}

cl_event oclmp_enqueue_add(
        oclmp_env env, 
        size_t count, 
        size_t a, size_t b, size_t c, 
        cl_mem A, cl_mem B, cl_mem C, 
        int num_events, cl_event **wait_list) {

    if (a != b) {
        std::cout << "Warning: addition with different operand sizes not implemented" << std::endl;
    }

    cl_kernel kernel = env.getKernel("add_sub", "oclmp_add");

    int n = a / 4;
    int m = c / 4;

    cl_int err = clSetKernelArg(kernel, 0, sizeof(int), &n);
    err |= clSetKernelArg(kernel, 1, sizeof(int), &m);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &A);
    err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &B);
    err |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &C);

    if (err != CL_SUCCESS) {
        throw std::runtime_error("Failed to set kernel arguments: Error: " + std::to_string(err));
    }

    cl_event evt;
    size_t global_work_size = count;

    cl_event wait_events[num_events];
    
    for (int i = 0; i < num_events; i++) {
        wait_events[i] = *wait_list[i];
    }

    err = clEnqueueNDRangeKernel(env.ocl.queue, kernel, 1, nullptr, &global_work_size, nullptr, num_events, wait_events, &evt);
    
    if (err != CL_SUCCESS) {
        throw std::runtime_error("Failed to enqueue kernel.");
    }

    return evt;
}

cl_event oclmp_enqueue_bitop(
        InstructionType bitop,
        oclmp_env env, 
        size_t count, 
        size_t a, size_t b, size_t c, 
        cl_mem A, cl_mem B, cl_mem C, 
        int num_events, cl_event **wait_list) {

    if (a != b) {
        std::cout << "Warning: bitop with different operand sizes not implemented" << std::endl;
    }

    if (a > c) {
        std::cout << "Warning: Bitop result might be too big for result type" << std::endl;
    }

    cl_kernel kernel;

    switch (bitop) {
        case InstructionType::BitAnd: {
            env.getKernel("bitops", "oclmp_bitwise_and");
            break;
        } case InstructionType::BitOr: {
            env.getKernel("bitops", "oclmp_bitwise_or");
            break;
        } default: {
            throw new std::runtime_error("Illegal bit op");
        }
    }     

    int n = a / 4;
    cl_int err = clSetKernelArg(kernel, 0, sizeof(int), &n);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &A);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &B);
    err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &C);

    if (err != CL_SUCCESS) {
        throw std::runtime_error("Failed to set kernel arguments: Error: " + std::to_string(err));
    }

    cl_event evt;
    const size_t global_work_size[1] = { count * n };

    cl_event wait_events[num_events];
    
    for (int i = 0; i < num_events; i++) {
        wait_events[i] = *wait_list[i];
    }

    err = clEnqueueNDRangeKernel(env.ocl.queue, kernel, 1, nullptr, global_work_size, nullptr, num_events, wait_events, &evt);
    
    if (err != CL_SUCCESS) {
        throw std::runtime_error("Failed to enqueue kernel.");
    }

    return evt;
}
