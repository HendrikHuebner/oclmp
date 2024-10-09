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

void load_oclmp(oclmp_env& env, oclmp* xs, int n) {
    cl_int err;
    for (int i = 0; i < n; i++) {
        if (!xs[i].cl_buf) {
            cl_mem buf = clCreateBuffer(env.ocl_manager.ctx, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, 
                (xs[i].frac_size + xs[i].int_size) * sizeof(u8), xs[i].data, &err);

            if (err != CL_SUCCESS) {
                printf("%d \n", err);

                throw std::runtime_error("Failed to create oclmp buffer on GPU.");
            }

            xs[i].cl_buf = buf;

        } else {

            // if already allocated, update gpu memory
            err = clEnqueueWriteBuffer(env.ocl_manager.queue, xs[i].cl_buf, false, 0, xs[i].size, xs[i].data, 0, nullptr, nullptr);
            if (err != CL_SUCCESS) {
                printf("%d \n", err);

                throw std::runtime_error("Failed to create enqueue write: Error " + std::to_string(err));
            }
        }
    }

    clFinish(env.ocl_manager.queue);
}

void load_oclmp(oclmp_env& env, oclmp& a) {
    load_oclmp(env, &a, 1);
}

void fetch_oclmp(oclmp_env& env, oclmp& a) {
    cl_int err;

    if (!a.cl_buf) {
        throw std::invalid_argument("No gpu buffer associated");
    }

    clEnqueueReadBuffer(env.ocl_manager.queue, a.cl_buf, CL_TRUE, 0, a.size * sizeof(unsigned char), a.data, 0, nullptr, nullptr);
    clFinish(env.ocl_manager.queue);
}

