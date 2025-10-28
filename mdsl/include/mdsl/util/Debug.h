#pragma once

#include "../ir/IRModule.h"
#include "../parser/ASTNode.h"

#include <iostream>
#include <sstream>
#include <string>

namespace mdsl
{
	namespace util
	{
		class ASTDumper
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
			ASTDumper() : indentLevel(0) {}

			void Dump(const parser::ASTNode* node)
			{
				if (!node)
				{
					WriteIndent();
					output << "<null>\n";
					return;
				}

				WriteIndent();
				output << "ASTNode(type=" << static_cast<int>(node->GetNodeType()) << ")\n";

				if (node->is<parser::Expression>())
				{
					WriteIndent();
					output << "  Expression\n";
				}
				else if (node->is<parser::Statement>())
				{
					WriteIndent();
					output << "  Statement\n";
				}
				else if (node->is<parser::Declaration>())
				{
					WriteIndent();
					output << "  Declaration\n";
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

		class IRDumper
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
			IRDumper() : indentLevel(0) {}

			void DumpInstruction(const ir::IRInstruction* instruction)
			{
				WriteIndent();
				output << "%" << instruction->GetId() << " = ";
				output << ir::IRInstruction::OpcodeToString(instruction->GetOpcode());

				for (size_t i = 0; i < instruction->GetNumOperands(); ++i)
				{
					output << " %" << instruction->GetOperand(i)->GetId();
				}

				if (instruction->GetNumSuccessors() > 0)
				{
					output << " -> [";
					for (size_t i = 0; i < instruction->GetNumSuccessors(); ++i)
					{
						if (i > 0)
						{
							output << ", ";
						}
						output << "bb" << instruction->GetSuccessors()[i];
					}
					output << "]";
				}

				output << "\n";
			}

			void DumpBasicBlock(const ir::IRBasicBlock* block)
			{
				WriteIndent();
				output << block->GetName() << ":\n";

				indentLevel++;
				for (const auto& instruction : block->GetInstructions())
				{
					DumpInstruction(instruction.get());
				}
				indentLevel--;
			}

			void DumpFunction(const ir::IRFunction* function)
			{
				output << "function " << function->GetName() << "\n";
				output << "{\n";

				indentLevel++;
				for (const auto& block : function->GetBasicBlocks())
				{
					DumpBasicBlock(block.get());
				}
				indentLevel--;

				output << "}\n\n";
			}

			void DumpModule(const ir::IRModule* module)
			{
				output << "module " << module->GetName() << "\n\n";

				if (!module->GetConstants().empty())
				{
					output << "constants:\n";
					indentLevel++;
					for (const auto& constant : module->GetConstants())
					{
						WriteIndent();
						output << "%" << constant->GetId() << " = const " << constant->GetValue() << "\n";
					}
					indentLevel--;
					output << "\n";
				}

				for (const auto& function : module->GetFunctions())
				{
					DumpFunction(function.get());
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

		class Logger
		{
		public:
			enum class Level
			{
				Debug,
				Info,
				Warning,
				Error
			};

		private:
			Level minLevel;

			const char* LevelToString(Level level)
			{
				switch (level)
				{
					case Level::Debug:
						return "DEBUG";
					case Level::Info:
						return "INFO";
					case Level::Warning:
						return "WARNING";
					case Level::Error:
						return "ERROR";
					default:
						return "UNKNOWN";
				}
			}

		public:
			Logger(Level level = Level::Info) : minLevel(level) {}

			void SetLevel(Level level)
			{
				minLevel = level;
			}

			void Log(Level level, const std::string& message)
			{
				if (level >= minLevel)
				{
					std::cout << "[" << LevelToString(level) << "] " << message << std::endl;
				}
			}

			void Debug(const std::string& message)
			{
				Log(Level::Debug, message);
			}

			void Info(const std::string& message)
			{
				Log(Level::Info, message);
			}

			void Warning(const std::string& message)
			{
				Log(Level::Warning, message);
			}

			void Error(const std::string& message)
			{
				Log(Level::Error, message);
			}
		};
	} // namespace util
} // namespace mdsl
