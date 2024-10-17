#include "computation.hpp"
#include "kernels.hpp"
#include "types.hpp"
#include <CL/cl.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <sys/wait.h>

using namespace std;

struct KernelInvocation {
    Instruction &inst;
    vector<cl_event*> wait_list;
    cl_event event;

  public:
    KernelInvocation(Instruction &inst, vector<cl_event*> wait_list) : 
        inst(inst), wait_list(wait_list) {}

    KernelInvocation(const KernelInvocation &copy) : inst(copy.inst), wait_list(copy.wait_list) {};
};

static void computeDataDependencies(vector<Instruction> &instructions, vector<unique_ptr<KernelInvocation>> &invocations) {
    for (auto &inst : instructions) {
        auto invocation = std::make_unique<KernelInvocation>(inst, vector<cl_event*>());
        invocations.push_back(std::move(invocation));
    }

    for (int i = instructions.size() - 1; i >= 0; i--) {
        oclmp_operand x = instructions[i].operand2;
        oclmp_operand *y = &instructions[i].operand3;
        
        bool found_x = false, found_y = false;

        for (int j = i - 1; j >= 0; j--) {
            oclmp_operand z = instructions[j].operand1;

            if (!found_x && x == z) {
                invocations[i]->wait_list.push_back(&invocations[j]->event);
                found_x = true;
                //std::cout << i << " x " << &invocations[j]->event << std::endl;
                if (found_y) 
                    break;

            } else if (!found_y && y && *y == z) {
                invocations[i]->wait_list.push_back(&invocations[j]->event);
                //std::cout << i << " y " << &invocations[j]->event << std::endl;
                found_y = true;

                if (found_x) 
                    break;
            }
        }
    }
    // TODO: Remove transient dependencies
}

void OclmpComputation::instCombine() {
    // TODO
}

void OclmpComputation::initTempOperands(oclmp_env env) {
    std::cout << temps.size() << std::endl;

    for (auto &temp : temps) {

        // TODO: Reuse buffers that are not in use at this point of the calculation

        cl_int err;
        temp.second = clCreateBuffer(env.ocl.ctx, CL_MEM_READ_WRITE   , 
            count * temp.first.size, nullptr, &err);
        
        std::cout << temp.second << std::endl;
        
        if (err != CL_SUCCESS) {
            throw std::runtime_error("Failed to create oclmp temp buffer on GPU: Error " + std::to_string(err));
        }
    }

    for (auto &inst : instructions) {
        if (temps[inst.operand1]) {
            inst.operand1.cl_buf = temps[inst.operand1];
        }
        if (temps[inst.operand2]) {
            inst.operand2.cl_buf = temps[inst.operand2];
        }
        if (temps[inst.operand3]) {
            inst.operand3.cl_buf = temps[inst.operand3];
        }
    }
}

void OclmpComputation::build(oclmp_env env) {
    instCombine();   
    initTempOperands(env);
    vector<unique_ptr<KernelInvocation>> invocations;
    computeDataDependencies(instructions, invocations);
    
    for (unique_ptr<KernelInvocation> &kernel : invocations) {
        switch (kernel->inst.type) {
            case InstructionType::Add: {
                kernel->event = oclmp_enqueue_add(env, count, 
                    kernel->inst.operand2.size, kernel->inst.operand3.size, kernel->inst.operand1.size, 
                    kernel->inst.operand2.cl_buf, kernel->inst.operand3.cl_buf, kernel->inst.operand1.cl_buf, 
                    kernel->wait_list.size(), kernel->wait_list.data());
                    continue;
            }
            case InstructionType::Multiply: {
                kernel->event = oclmp_enqueue_mul(env, count, 
                    kernel->inst.operand2.size, kernel->inst.operand3.size, kernel->inst.operand1.size, 
                    kernel->inst.operand2.cl_buf, kernel->inst.operand3.cl_buf, kernel->inst.operand1.cl_buf, 
                    kernel->wait_list.size(), kernel->wait_list.data());
                    continue;
            }
            case InstructionType::BitAnd:
            case InstructionType::BitOr: {
                kernel->event = oclmp_enqueue_bitop(kernel->inst.type, env, count, 
                    kernel->inst.operand2.size, kernel->inst.operand3.size, kernel->inst.operand1.size, 
                    kernel->inst.operand2.cl_buf, kernel->inst.operand3.cl_buf, kernel->inst.operand1.cl_buf, 
                    kernel->wait_list.size(), kernel->wait_list.data());
                    continue;
            }

            default: {
                throw runtime_error("Not implemented");
            }
        }
    }
}
