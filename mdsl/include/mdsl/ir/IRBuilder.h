#pragma once

#include "IRModule.h"

#include <memory>

namespace mdsl
{
	namespace ir
	{
		class IRBuilder
		{
		private:
			IRModule*	  module;
			IRFunction*	  currentFunction;
			IRBasicBlock* currentBlock;
			uint32_t	  nextValueId;

		public:
			IRBuilder() : module(nullptr), currentFunction(nullptr), currentBlock(nullptr), nextValueId(0) {}

			void SetModule(IRModule* mod)
			{
				module = mod;
			}

			IRModule* GetModule() const
			{
				return module;
			}

			void SetInsertPoint(IRBasicBlock* block)
			{
				currentBlock = block;
			}

			IRBasicBlock* GetInsertPoint() const
			{
				return currentBlock;
			}

			void SetCurrentFunction(IRFunction* func)
			{
				currentFunction = func;
			}

			IRFunction* GetCurrentFunction() const
			{
				return currentFunction;
			}

			IRInstruction* CreateInstruction(IROpcode opcode)
			{
				auto		   instr = std::make_unique<IRInstruction>(opcode, nextValueId++);
				IRInstruction* result = instr.get();

				if (currentBlock)
				{
					currentBlock->AddInstruction(std::move(instr));
				}

				return result;
			}

			IRInstruction* CreateBinaryOp(IROpcode opcode, IRValue* lhs, IRValue* rhs)
			{
				IRInstruction* instr = CreateInstruction(opcode);
				instr->AddOperand(lhs);
				instr->AddOperand(rhs);
				return instr;
			}

			IRInstruction* CreateAdd(IRValue* lhs, IRValue* rhs)
			{
				return CreateBinaryOp(IROpcode::Add, lhs, rhs);
			}

			IRInstruction* CreateSub(IRValue* lhs, IRValue* rhs)
			{
				return CreateBinaryOp(IROpcode::Sub, lhs, rhs);
			}

			IRInstruction* CreateMul(IRValue* lhs, IRValue* rhs)
			{
				return CreateBinaryOp(IROpcode::Mul, lhs, rhs);
			}

			IRInstruction* CreateDiv(IRValue* lhs, IRValue* rhs)
			{
				return CreateBinaryOp(IROpcode::Div, lhs, rhs);
			}

			IRInstruction* CreateLoad(IRValue* address)
			{
				IRInstruction* instr = CreateInstruction(IROpcode::Load);
				instr->AddOperand(address);
				return instr;
			}

			IRInstruction* CreateStore(IRValue* value, IRValue* address)
			{
				IRInstruction* instr = CreateInstruction(IROpcode::Store);
				instr->AddOperand(value);
				instr->AddOperand(address);
				return instr;
			}

			IRInstruction* CreateCall(IRValue* function, const std::vector<IRValue*>& args)
			{
				IRInstruction* instr = CreateInstruction(IROpcode::Call);
				instr->AddOperand(function);

				for (IRValue* arg : args)
				{
					instr->AddOperand(arg);
				}

				return instr;
			}

			IRInstruction* CreateReturn(IRValue* value = nullptr)
			{
				IRInstruction* instr = CreateInstruction(IROpcode::Return);
				if (value)
				{
					instr->AddOperand(value);
				}
				return instr;
			}

			IRInstruction* CreateJump(IRBasicBlock* target)
			{
				IRInstruction* instr = CreateInstruction(IROpcode::Jump);
				instr->AddSuccessor(target->GetId());
				return instr;
			}

			IRInstruction* CreateConditionalJump(
				IRValue* condition, IRBasicBlock* trueBlock, IRBasicBlock* falseBlock)
			{
				IRInstruction* instr = CreateInstruction(IROpcode::JumpIf);
				instr->AddOperand(condition);
				instr->AddSuccessor(trueBlock->GetId());
				instr->AddSuccessor(falseBlock->GetId());
				return instr;
			}

			IRInstruction* CreateAlloca()
			{
				IRInstruction* instr = CreateInstruction(IROpcode::Alloca);
				return instr;
			}

			IRConstant* CreateConstant(int64_t value)
			{
				if (!module)
				{
					return nullptr;
				}
				return module->CreateConstant(value, nextValueId++);
			}

			void Reset()
			{
				nextValueId = 0;
				currentFunction = nullptr;
				currentBlock = nullptr;
			}
		};
	} // namespace ir
} // namespace mdsl
