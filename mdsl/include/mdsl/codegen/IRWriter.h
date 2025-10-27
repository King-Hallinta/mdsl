#pragma once

#include "../ir/IRModule.h"

#include <sstream>
#include <string>

namespace mdsl
{
	namespace codegen
	{
		class IRWriter
		{
		private:
			std::ostringstream output;
			int				   indentLevel;

			void WriteIndent()
			{
				for (int i = 0; i < indentLevel; ++i)
				{
					output << "  ";
				}
			}

		public:
			IRWriter() : indentLevel(0) {}

			void WriteInstruction(const ir::IRInstruction* instruction)
			{
				WriteIndent();
				output << instruction->ToString();

				for (size_t i = 0; i < instruction->GetNumOperands(); ++i)
				{
					output << " %" << instruction->GetOperand(i)->GetId();
				}

				output << "\n";
			}

			void WriteBasicBlock(const ir::IRBasicBlock* block)
			{
				WriteIndent();
				output << block->GetName() << ":\n";

				indentLevel++;
				for (const auto& instruction : block->GetInstructions())
				{
					WriteInstruction(instruction.get());
				}
				indentLevel--;
			}

			void WriteFunction(const ir::IRFunction* function)
			{
				output << "function " << function->GetName() << "(";

				const auto& params = function->GetParameters();
				for (size_t i = 0; i < params.size(); ++i)
				{
					if (i > 0)
					{
						output << ", ";
					}
					output << params[i];
				}

				output << ")\n";
				output << "{\n";

				indentLevel++;
				for (const auto& block : function->GetBasicBlocks())
				{
					WriteBasicBlock(block.get());
				}
				indentLevel--;

				output << "}\n\n";
			}

			void WriteModule(const ir::IRModule* module)
			{
				output << "module " << module->GetName() << "\n\n";

				for (const auto& function : module->GetFunctions())
				{
					WriteFunction(function.get());
				}
			}

			std::string ToString() const
			{
				return output.str();
			}

			void Clear()
			{
				output.str("");
				output.clear();
				indentLevel = 0;
			}
		};
	} // namespace codegen
} // namespace mdsl
