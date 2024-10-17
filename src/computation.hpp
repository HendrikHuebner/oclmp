#pragma once

#include "oclmp.hpp"
#include "types.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

enum class InstructionType {
    Add,
    Subtract,
    Multiply,
    BitAnd,
    BitOr,
};

struct Instruction {
    InstructionType type;
    oclmp_operand operand1;
    oclmp_operand operand2;
    oclmp_operand operand3;

    std::vector<Instruction> waitList;

    Instruction(InstructionType t, oclmp_operand op1, oclmp_operand op2, oclmp_operand op3)
        : type(t), operand1(std::move(op1)), operand2(std::move(op2)), operand3(std::move(op3)) {};
};

class OclmpComputation {
    std::unordered_map<oclmp_operand, cl_mem> temps;
    std::vector<Instruction> instructions;
    size_t count;
    
private:

    inline void registerTempOperand(oclmp_operand &op) {
        if (!op.cl_buf) {
            temps[op] = nullptr;
        }
    }

    void addBinOp(InstructionType type, oclmp_operand a, oclmp_operand b, oclmp_operand c) {
        registerTempOperand(a);
        registerTempOperand(b);
        registerTempOperand(c);
        instructions.emplace_back(type, c, a, b);
    }

public:

    OclmpComputation(size_t count) : count(count) {}
    
    void addAddition(oclmp_operand a, oclmp_operand b, oclmp_operand c) {
        addBinOp(InstructionType::Add, a, b, c);
    }

    void addMultiplication(oclmp_operand a, oclmp_operand b, oclmp_operand c) {
        addBinOp(InstructionType::Multiply, a, b, c);
    }

    void addBitwiseAnd(oclmp_operand a, oclmp_operand b, oclmp_operand c) {
        addBinOp(InstructionType::BitAnd, a, b, c);
    }

    void addBitwiseOr(oclmp_operand a, oclmp_operand b, oclmp_operand c) {
        addBinOp(InstructionType::BitOr, a, b, c);
    }

    void build(oclmp_env env);
  
  protected:
    void instCombine();

    void initTempOperands(oclmp_env env);
};
