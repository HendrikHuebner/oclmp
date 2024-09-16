#include <CL/cl.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vector>

class OCLManager {
  public:
    cl_context ctx;
    cl_device_id dev;
    cl_platform_id platform;
    cl_command_queue queue;

    std::filesystem::path ocl_folder;

    OCLManager(std::string ocl_folder) {
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

    cl_program build_program(std::string filename) {
        cl_int err;
        
        std::ifstream f(ocl_folder / filename, std::ios::in);
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
};


