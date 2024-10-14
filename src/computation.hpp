#pragma once

#include "oclmp.hpp"
#include "types.hpp"
#include <memory>
#include <vector>


using namespace std;

enum class InstructionType {
    Add,
    Subtract,
    Multiply,
};

struct Operand {
    long id;

    Operand(long id) : id(id) {}

    virtual ~Operand() {}

  public:
    virtual cl_mem mem() = 0;
    virtual int size() = 0;
};

struct DataSourceOperand : public Operand {
    cl_mem buf;
    int _size;

    DataSourceOperand(oclmp_data data) : Operand(data.id), buf(data.src->cl_buf), _size(data.src->size) {}
    
    ~DataSourceOperand() {}

  public:
    cl_mem mem() override {
        return buf;
    }

    int size() override {
        return _size;   
    }
};

struct Instruction {
    InstructionType type;
    std::unique_ptr<Operand> operand1;
    std::unique_ptr<Operand> operand2;
    std::unique_ptr<Operand> operand3;

    vector<shared_ptr<Instruction>> waitList;

    Instruction(InstructionType t, std::unique_ptr<Operand> op1, std::unique_ptr<Operand> op2, std::unique_ptr<Operand> op3)
        : type(t), operand1(std::move(op1)), operand2(std::move(op2)), operand3(std::move(op3)) {}
};


class OclmpComputation {
    vector<oclmp_data> data_sources;
    vector<oclmp_temp> temps;
    vector<Instruction> instructions;
    size_t count;
    
public:

    OclmpComputation(size_t count) : count(count) {}
    
    void addAddition(oclmp_data a, oclmp_data b, oclmp_data c) {
        std::unique_ptr<Operand> op1 = std::make_unique<DataSourceOperand>(c);
        std::unique_ptr<Operand> op2 = std::make_unique<DataSourceOperand>(a);
        std::unique_ptr<Operand> op3 = std::make_unique<DataSourceOperand>(b);

        instructions.emplace_back(InstructionType::Add, std::move(op1), std::move(op2), std::move(op3));
    }

    void addMultiplication(oclmp_data a, oclmp_data b, oclmp_data c) {
        std::unique_ptr<Operand> op1 = std::make_unique<DataSourceOperand>(c);
        std::unique_ptr<Operand> op2 = std::make_unique<DataSourceOperand>(a);
        std::unique_ptr<Operand> op3 = std::make_unique<DataSourceOperand>(b);

        instructions.emplace_back(InstructionType::Multiply, std::move(op1), std::move(op2), std::move(op3));
    }

    void build(oclmp_env env);
  
  protected:
    void instCombine();
};
