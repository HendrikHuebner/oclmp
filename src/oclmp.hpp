#pragma once

#include <CL/cl.h>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include "ocl_manager.hpp"
#include "types.hpp"

struct oclmp_context {
    OCLManager ocl_manager;
    std::vector<cl_mem> buffers;

    cl_program bitwise_ops;

    oclmp_context(std::string path) : ocl_manager(path) {
        bitwise_ops = ocl_manager.build_program("bitops.cl");
    }

    void close() {
        for (auto buf : buffers) {
            clReleaseMemObject(buf);
        }

        clReleaseContext(ocl_manager.ctx);
        clReleaseDevice(ocl_manager.dev);
    }

    cl_kernel getKernel(std::string name) {
        cl_kernel kernel = clCreateKernel(bitwise_ops, "oclmp_bitwise_or", nullptr);
        return kernel;
    }
};

void load_oclmp(OCLManager& m, oclmp_t& a) {
    cl_int err;

    if (!a.cl_buf) {
        cl_mem buf = clCreateBuffer(m.ctx, CL_MEM_READ_WRITE, 
            (a.frac_size + a.int_size) * sizeof(b256int_t), a.data, &err);

        if (err != CL_SUCCESS) {
            throw std::runtime_error("Failed to create oclmp buffer on GPU.");
        }

        a.cl_buf = buf;
    } else {
        // if already allocated, update gpu memory
        err = clEnqueueWriteBuffer(m.queue, a.cl_buf, true, 0, a.size, a.data, 0, nullptr, nullptr);

    }

}

void fetch_oclmp(OCLManager& m, oclmp_t& a) {
    cl_int err;

    if (!a.cl_buf) {
        throw std::invalid_argument("No gpu buffer associated");
    }

    clEnqueueReadBuffer(m.queue, a.cl_buf, true, 0, a.size, a.data, 0, nullptr, nullptr);
}

void clear_oclmp(OCLManager& m, oclmp_t& a) {
    cl_int err;

    if (!a.cl_buf) {
        throw std::invalid_argument("No gpu buffer associated");
    }
    
    delete[] a.data;

    err = clReleaseMemObject(a.cl_buf);
    if (err != CL_SUCCESS) {
        throw std::runtime_error("Failed to create oclmp buffer on GPU.");
    }
}

void oclmp_bitwise_or(oclmp_context ctx, oclmp_t& a, oclmp_t& b, oclmp_t& c) {
    
    cl_command_queue queue = clCreateCommandQueueWithProperties(ctx.ocl_manager.ctx, ctx.ocl_manager.dev, 0, nullptr);
    cl_kernel kernel = ctx.getKernel("");

    cl_int err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &a.cl_buf);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &b.cl_buf);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &c.cl_buf);

    if (err != CL_SUCCESS) {
        throw std::runtime_error("Failed to set kernel arguments.");
    }

    size_t global_work_size = c.size; 
    err = clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &global_work_size, nullptr, 0, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        throw std::runtime_error("Failed to enqueue kernel.");
    }

    clFinish(queue);
}
