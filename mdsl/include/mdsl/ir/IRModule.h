#pragma once

#include "IRInstruction.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace mdsl
{
	namespace ir
	{
		class IRBasicBlock
		{
		private:
			std::string									name;
			std::vector<std::unique_ptr<IRInstruction>> instructions;
			uint32_t									id;

		public:
			IRBasicBlock(const std::string& n, uint32_t blockId) : name(n), id(blockId) {}

			const std::string& GetName() const
			{
				return name;
			}

			uint32_t GetId() const
			{
				return id;
			}

			void AddInstruction(std::unique_ptr<IRInstruction> instruction)
			{
				instructions.push_back(std::move(instruction));
			}

			const std::vector<std::unique_ptr<IRInstruction>>& GetInstructions() const
			{
				return instructions;
			}

			size_t GetInstructionCount() const
			{
				return instructions.size();
			}
		};

		class IRFunction
		{
		private:
			std::string								   name;
			std::vector<std::unique_ptr<IRBasicBlock>> basicBlocks;
			std::vector<std::string>				   parameters;
			uint32_t								   nextBlockId;

		public:
			IRFunction(const std::string& n) : name(n), nextBlockId(0) {}

			const std::string& GetName() const
			{
				return name;
			}

			IRBasicBlock* CreateBasicBlock(const std::string& blockName)
			{
				basicBlocks.push_back(std::make_unique<IRBasicBlock>(blockName, nextBlockId++));
				return basicBlocks.back().get();
			}

			const std::vector<std::unique_ptr<IRBasicBlock>>& GetBasicBlocks() const
			{
				return basicBlocks;
			}

			void AddParameter(const std::string& param)
			{
				parameters.push_back(param);
			}

			const std::vector<std::string>& GetParameters() const
			{
				return parameters;
			}

			size_t GetBasicBlockCount() const
			{
				return basicBlocks.size();
			}
		};

		class IRModule
		{
		private:
			std::string									 name;
			std::vector<std::unique_ptr<IRFunction>>	 functions;
			std::unordered_map<std::string, IRFunction*> functionMap;

		public:
			IRModule(const std::string& n = "module") : name(n) {}

			const std::string& GetName() const
			{
				return name;
			}

			IRFunction* CreateFunction(const std::string& functionName)
			{
				functions.push_back(std::make_unique<IRFunction>(functionName));
				IRFunction* func = functions.back().get();
				functionMap[functionName] = func;
				return func;
			}

			IRFunction* GetFunction(const std::string& functionName)
			{
				auto it = functionMap.find(functionName);
				if (it != functionMap.end())
				{
					return it->second;
				}
				return nullptr;
			}

			const std::vector<std::unique_ptr<IRFunction>>& GetFunctions() const
			{
				return functions;
			}

			size_t GetFunctionCount() const
			{
				return functions.size();
			}
		};
	} // namespace ir
} // namespace mdsl
