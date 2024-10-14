#include "oclmp.hpp"
#include "computation.hpp"
#include <CL/cl.h>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include "types.hpp"
#include <filesystem>
#include <string>
#include <fstream>

cl_program ocl_manager::build_program(std::string filename) {
    cl_int err;
    std::filesystem::path p = ocl_folder / filename;

    std::ifstream f(p, std::ios::in );

    if (!f.is_open())
        throw std::runtime_error("Failed to open kernel file.");
    
    std::string kernel_source = std::string(std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());
    const char* cstr = kernel_source.c_str();
    size_t source_size = kernel_source.size();

    cl_program program = clCreateProgramWithSource(ctx, 1, &cstr, &source_size, &err);
    if (err != CL_SUCCESS) {
        throw std::runtime_error("Failed to create OpenCL program.");
    }

    err = clBuildProgram(program, 1, &dev, nullptr, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        size_t log_size;
        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size);
        std::vector<char> log(log_size);
        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, log_size, log.data(), nullptr);
        std::cerr << "Build log:\n" << log.data() << "\n";
        throw std::runtime_error("Failed to build OpenCL program.");
    }

    return program;
}

static std::unique_ptr<OclmpComputation> computation;

void oclmp_begin(oclmp_env &env, size_t count) {
    computation = std::make_unique<OclmpComputation>(count);
}

void oclmp_run(oclmp_env &env) {
    computation->build(env);
    clFinish(env.ocl_manager.queue);
}

void oclmp_add(oclmp_env ctx, oclmp_data& a, oclmp_data& b, oclmp_data& c) {
    if (!computation)
        throw std::runtime_error("Not currently in a OCLMP computation!");

    computation->addAddition(a, b, c);
}

void oclmp_mul(oclmp_env ctx, oclmp_data& a, oclmp_data& b, oclmp_data& c) {
    if (!computation)
        throw std::runtime_error("Not currently in a OCLMP computation!");

    computation->addMultiplication(a, b, c);
}

struct sub_buffer_region {
    size_t origin;
    size_t size;
};

void oclmp_load_pool(oclmp_env& env, oclmp_pool& pool) {
    cl_int err;
    cl_mem buf = clCreateBuffer(env.ocl_manager.ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
        pool.count * pool.size * sizeof(u8), pool.data, &err);

    if (err != CL_SUCCESS) {
        throw std::runtime_error("Failed to create oclmp buffer on GPU: Error " + std::to_string(err));
    }
    
    pool.cl_buf = buf;

    /*
    for (int i = 0; i < pool.count; i++) {
        sub_buffer_region region = {
            .origin = i * pool.size,
            .size = pool.size
        };

        pool[i].cl_buf = clCreateSubBuffer(buf, CL_MEM_READ_WRITE,
                                           CL_BUFFER_CREATE_TYPE_REGION, &region, &err);
    } */
    

    clFinish(env.ocl_manager.queue);
}

void oclmp_fetch_pool(oclmp_env& env, oclmp_pool &pool) {
    cl_int err;

    if (!pool.cl_buf) {
        throw std::invalid_argument("No gpu buffer associated");
    }

    clEnqueueReadBuffer(env.ocl_manager.queue, pool.cl_buf, CL_TRUE, 0, pool.size * pool.count, pool.data, 0, nullptr, nullptr);
    clFinish(env.ocl_manager.queue);
}
