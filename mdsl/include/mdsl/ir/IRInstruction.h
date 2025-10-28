#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace mdsl
{
	namespace ir
	{
		enum class IROpcode
		{
			Nop,

			Load,
			Store,

			Add,
			Sub,
			Mul,
			Div,
			Mod,

			And,
			Or,
			Xor,
			Not,

			Shl,
			Shr,

			Eq,
			Ne,
			Lt,
			Le,
			Gt,
			Ge,

			Jump,
			JumpIf,
			JumpIfNot,

			Call,
			Return,

			Phi,

			Alloca,

			UserDefinedStart = 1000
		};

		class IRValue
		{
		protected:
			std::string name;
			uint32_t	id;

		public:
			IRValue(const std::string& n = "", uint32_t valueId = 0) : name(n), id(valueId) {}

			virtual ~IRValue() = default;

			const std::string& GetName() const
			{
				return name;
			}

			uint32_t GetId() const
			{
				return id;
			}

			void SetName(const std::string& n)
			{
				name = n;
			}
		};

		class IRInstruction : public IRValue
		{
		protected:
			IROpcode			  opcode;
			std::vector<IRValue*> operands;
			std::vector<uint32_t> successorBlocks;

		public:
			IRInstruction(IROpcode op, uint32_t id = 0) : IRValue("", id), opcode(op) {}

			IROpcode GetOpcode() const
			{
				return opcode;
			}

			void AddOperand(IRValue* operand)
			{
				operands.push_back(operand);
			}

			const std::vector<IRValue*>& GetOperands() const
			{
				return operands;
			}

			IRValue* GetOperand(size_t index) const
			{
				if (index < operands.size())
				{
					return operands[index];
				}
				return nullptr;
			}

			size_t GetNumOperands() const
			{
				return operands.size();
			}

			void AddSuccessor(uint32_t blockId)
			{
				successorBlocks.push_back(blockId);
			}

			const std::vector<uint32_t>& GetSuccessors() const
			{
				return successorBlocks;
			}

			size_t GetNumSuccessors() const
			{
				return successorBlocks.size();
			}

			virtual std::string ToString() const
			{
				return std::string("%") + std::to_string(id) + " = " + OpcodeToString(opcode);
			}

			static std::string OpcodeToString(IROpcode op)
			{
				switch (op)
				{
					case IROpcode::Nop:
						return "nop";
					case IROpcode::Load:
						return "load";
					case IROpcode::Store:
						return "store";
					case IROpcode::Add:
						return "add";
					case IROpcode::Sub:
						return "sub";
					case IROpcode::Mul:
						return "mul";
					case IROpcode::Div:
						return "div";
					case IROpcode::Mod:
						return "mod";
					case IROpcode::And:
						return "and";
					case IROpcode::Or:
						return "or";
					case IROpcode::Xor:
						return "xor";
					case IROpcode::Not:
						return "not";
					case IROpcode::Shl:
						return "shl";
					case IROpcode::Shr:
						return "shr";
					case IROpcode::Eq:
						return "eq";
					case IROpcode::Ne:
						return "ne";
					case IROpcode::Lt:
						return "lt";
					case IROpcode::Le:
						return "le";
					case IROpcode::Gt:
						return "gt";
					case IROpcode::Ge:
						return "ge";
					case IROpcode::Jump:
						return "jump";
					case IROpcode::JumpIf:
						return "jumpif";
					case IROpcode::JumpIfNot:
						return "jumpifnot";
					case IROpcode::Call:
						return "call";
					case IROpcode::Return:
						return "return";
					case IROpcode::Phi:
						return "phi";
					case IROpcode::Alloca:
						return "alloca";
					default:
						return "unknown";
				}
			}
		};

		class IRConstant : public IRValue
		{
		private:
			int64_t value;

		public:
			IRConstant(int64_t val, uint32_t id = 0) : IRValue("", id), value(val) {}

			int64_t GetValue() const
			{
				return value;
			}
		};

	} // namespace ir
} // namespace mdsl
