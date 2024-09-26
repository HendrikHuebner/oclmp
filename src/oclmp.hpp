#pragma once

#include <CL/cl.h>
#include <filesystem>
#include <string>
#include <unordered_map>
#include "types.hpp"


class ocl_manager {
  public:
    cl_context ctx;
    cl_device_id dev;
    cl_platform_id platform;
    cl_command_queue queue;

    std::filesystem::path ocl_folder;

    ocl_manager(std::string ocl_folder) {
        cl_int err;

        this->ocl_folder = ocl_folder;
        if (!std::filesystem::exists(ocl_folder)) { 
            throw std::invalid_argument("Opencl source directory does not exist.");
        }

        err = clGetPlatformIDs(1, &platform, nullptr);
        if (err != CL_SUCCESS) {
            throw std::runtime_error("Failed to find an OpenCL platform.");
        }

        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, nullptr);
        if (err != CL_SUCCESS) {
            throw std::runtime_error("Failed to find a GPU device.");
        }

        ctx = clCreateContext(nullptr, 1, &dev, nullptr, nullptr, &err);
        if (err != CL_SUCCESS) {
            throw std::runtime_error("Failed to create OpenCL context.");
        }

        queue = clCreateCommandQueueWithProperties(ctx, dev, 0, &err);
        if (err != CL_SUCCESS) {
            throw std::runtime_error("Failed to create OpenCL queue.");
        }
    }

    cl_program build_program(std::string filename);
};

struct oclmp_env {
    ocl_manager ocl_manager;
    std::unordered_map<std::string, cl_kernel> kernels;
    std::unordered_map<std::string, cl_program> programs;

    oclmp_env(std::string path) : ocl_manager(path) {}

    void close() {
        for (auto kernel : kernels) 
            clReleaseKernel(kernel.second);

        clReleaseContext(ocl_manager.ctx);
        clReleaseDevice(ocl_manager.dev);
    }

    cl_program getProgram(std::string name) {
        if (programs.find(name) != programs.end()) 
            programs.at(name);

        cl_program program = ocl_manager.build_program(name + ".cl");
        programs[name] = program;
        return program;
    }

    cl_kernel getKernel(std::string file, std::string name) {
        if (kernels.find(name) != kernels.end()) 
            kernels.at(name);

        cl_kernel kernel = clCreateKernel(getProgram(file), name.c_str(), nullptr);
        kernels[name] = kernel;
        return kernel;
    }
};

void oclmp_run(oclmp_env& env);

void load_oclmp(oclmp_env& env, oclmp& a);

void load_oclmp(oclmp_env& env, oclmp* a, int n);

void fetch_oclmp(oclmp_env& env, oclmp& a);

void clear_oclmp(oclmp_env& env, oclmp& a);

void oclmp_bitwise_or(oclmp_env ctx, oclmp& a, oclmp& b, oclmp& c);

void oclmp_add(oclmp_env ctx, oclmp& a, oclmp& b, oclmp& c);

void oclmp_mul(oclmp_env ctx, oclmp& a, oclmp& b, oclmp& c);

bool oclmp_cmp(oclmp_env ctx, oclmp& a);
