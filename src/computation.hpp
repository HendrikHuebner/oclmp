#pragma once

#include "oclmp.hpp"
#include "types.hpp"
#include <memory>
#include <optional>
#include <vector>
#include <string>

using namespace std;

enum class InstructionType {
    Add,
    Subtract,
    Multiply,
};

struct Operand {
    long id;

    Operand(long id) : id(id) {}

    virtual cl_mem mem() { return nullptr; };
    virtual int size() { return 0; };
};

struct DataSourceOperand : public Operand {
    cl_mem buf;
    int _size;

    DataSourceOperand(oclmp_data data) : Operand(data.id), buf(data.src->cl_buf), _size(data.src->size) {}

    cl_mem mem() override {
        return buf;
    }

    int size() override {
        return _size;   
    }
};

struct Instruction {
    InstructionType type;
    Operand operand1;
    Operand operand2;
    optional<Operand> operand3;

    vector<shared_ptr<Instruction>> waitList;

    Instruction(InstructionType t, Operand op1, Operand op2, optional<Operand> op3 = nullopt)
        : type(t), operand1(op1), operand2(op2), operand3(op3) {}
};

class OclmpComputation {
    vector<oclmp_data> data_sources;
    vector<oclmp_temp> temps;
    vector<Instruction> instructions;
    size_t count;
    
public:

    OclmpComputation(size_t count) : count(count) {}
    
    void addAddition(oclmp_data a, oclmp_data b, oclmp_data c) {
        DataSourceOperand op1(c);
        DataSourceOperand op2(a);
        DataSourceOperand op3(b);
        instructions.emplace_back(InstructionType::Add, op1, op2, optional<Operand>(op3));
    }

    void addMultiplication(oclmp_data a, oclmp_data b, oclmp_data c) {
        DataSourceOperand op1(c);
        DataSourceOperand op2(a);
        DataSourceOperand op3(b);
        instructions.emplace_back(InstructionType::Multiply, op1, op2, optional<Operand>(op3));
    }

    void build(oclmp_env env);
  
  protected:
    void instCombine();

    // debug 

    string operandToString(const Operand& operand) const {
        return "";
    }

    string toString() const {
        string result;
        for (size_t i = 0; i < instructions.size(); ++i) {
            const auto& instr = instructions[i];
            result += "res" + to_string(i) + " = ";

            switch (instr.type) {
                case InstructionType::Add:
                    result += operandToString(instr.operand1) + " Add " + operandToString(instr.operand2);
                    break;
                case InstructionType::Subtract:
                    result += operandToString(instr.operand1) + " Sub " + operandToString(instr.operand2);
                    break;
                case InstructionType::Multiply:
                    result += operandToString(instr.operand1) + " Mul " + operandToString(instr.operand2);
                    break;
            }
            result += "\n";
        }
        return result;
    }
};
