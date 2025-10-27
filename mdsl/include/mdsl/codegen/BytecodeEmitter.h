#pragma once

#include "../ir/IRModule.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

namespace mdsl
{
	namespace codegen
	{
		class BytecodeEmitter
		{
		private:
			std::vector<uint8_t>				 bytecode;
			std::unordered_map<uint32_t, size_t> valueToOffset;

		public:
			BytecodeEmitter() {}

			void Emit(uint8_t byte)
			{
				bytecode.push_back(byte);
			}

			void Emit(uint16_t value)
			{
				Emit(static_cast<uint8_t>(value & 0xFF));
				Emit(static_cast<uint8_t>((value >> 8) & 0xFF));
			}

			void Emit(uint32_t value)
			{
				Emit(static_cast<uint16_t>(value & 0xFFFF));
				Emit(static_cast<uint16_t>((value >> 16) & 0xFFFF));
			}

			void Emit(uint64_t value)
			{
				Emit(static_cast<uint32_t>(value & 0xFFFFFFFF));
				Emit(static_cast<uint32_t>((value >> 32) & 0xFFFFFFFF));
			}

			void Emit(int8_t value)
			{
				Emit(static_cast<uint8_t>(value));
			}

			void Emit(int16_t value)
			{
				Emit(static_cast<uint16_t>(value));
			}

			void Emit(int32_t value)
			{
				Emit(static_cast<uint32_t>(value));
			}

			void Emit(int64_t value)
			{
				Emit(static_cast<uint64_t>(value));
			}

			void EmitOpcode(ir::IROpcode opcode)
			{
				Emit(static_cast<uint8_t>(opcode));
			}

			void EmitInstruction(const ir::IRInstruction* instruction)
			{
				EmitOpcode(instruction->GetOpcode());

				for (size_t i = 0; i < instruction->GetNumOperands(); ++i)
				{
					ir::IRValue* operand = instruction->GetOperand(i);
					Emit(operand->GetId());
				}
			}

			void EmitFunction(const ir::IRFunction* function)
			{
				for (const auto& block : function->GetBasicBlocks())
				{
					for (const auto& instruction : block->GetInstructions())
					{
						EmitInstruction(instruction.get());
					}
				}
			}

			void EmitModule(const ir::IRModule* module)
			{
				Emit(static_cast<uint32_t>(module->GetFunctionCount()));

				for (const auto& function : module->GetFunctions())
				{
					EmitFunction(function.get());
				}
			}

			const std::vector<uint8_t>& GetBytecode() const
			{
				return bytecode;
			}

			void Clear()
			{
				bytecode.clear();
				valueToOffset.clear();
			}

			size_t Size() const
			{
				return bytecode.size();
			}
		};

	} // namespace codegen
} // namespace mdsl
