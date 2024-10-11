#include "computation.hpp"
#include "kernels.hpp"
#include <CL/cl.h>
#include <stdexcept>
#include <sys/wait.h>
#include <iostream>

struct KernelInvocation {
    Instruction &inst;
    vector<cl_event*> wait_list;
    cl_event event;

    KernelInvocation(Instruction &inst, vector<cl_event*> wait_list) : 
        inst(inst), wait_list(wait_list) {}
};

static void computeDataDependencies(vector<Instruction> &instructions, vector<KernelInvocation> &invocations) {
    for (auto &inst : instructions) {
        invocations.emplace_back(inst, vector<cl_event*>());
    }

    for (int i = instructions.size() - 1; i >= 0; i--) {
        auto &x = instructions[i].operand2;
        auto &y = instructions[i].operand3;
        
        bool found_x = false, found_y = false;

        for (int j = i - 1; j >= 0; j--) {
            auto &z = instructions[j].operand1;

            if (!found_x && x->id == z->id) {
                invocations[i].wait_list.push_back(&invocations[j].event);
                found_x = true;
                if (found_y) 
                    break;

            } else if (!found_y && y && y->id == z->id) {
                invocations[i].wait_list.push_back(&invocations[j].event);
                found_y = true;

                if (found_x) 
                    break;
            }
        }
    }

    // TODO: Remove transient dependencies
}

void OclmpComputation::instCombine() {

}


void OclmpComputation::build(oclmp_env env) {
    instCombine();
    vector<KernelInvocation> invocations;
    computeDataDependencies(instructions, invocations);

    for (KernelInvocation kernel : invocations) {
        switch (kernel.inst.type) {
            case InstructionType::Add: {
                kernel.event = oclmp_enqueue_add(env, count, 
                    kernel.inst.operand2->size(), kernel.inst.operand3->size(), kernel.inst.operand1->size(), 
                    kernel.inst.operand2->mem(), kernel.inst.operand3->mem(), kernel.inst.operand1->mem(), 
                    kernel.wait_list.size(), kernel.wait_list.data());
                    continue;
            }
            default: {
                throw runtime_error("Not implemented");
            }
        }
    }
}
