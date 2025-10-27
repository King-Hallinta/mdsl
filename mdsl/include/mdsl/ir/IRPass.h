#pragma once

#include "IRModule.h"

namespace mdsl
{
	namespace ir
	{
		class IRPass
		{
		public:
			virtual ~IRPass() = default;

			virtual bool RunOnModule(IRModule* module) = 0;

			virtual const char* GetPassName() const = 0;
		};

		class IRFunctionPass : public IRPass
		{
		public:
			virtual bool RunOnFunction(IRFunction* function) = 0;

			bool RunOnModule(IRModule* module) override
			{
				bool changed = false;

				for (const auto& func : module->GetFunctions())
				{
					if (RunOnFunction(func.get()))
					{
						changed = true;
					}
				}

				return changed;
			}
		};

		class IRBasicBlockPass : public IRFunctionPass
		{
		public:
			virtual bool RunOnBasicBlock(IRBasicBlock* block) = 0;

			bool RunOnFunction(IRFunction* function) override
			{
				bool changed = false;

				for (const auto& block : function->GetBasicBlocks())
				{
					if (RunOnBasicBlock(block.get()))
					{
						changed = true;
					}
				}

				return changed;
			}
		};

		class IRPassManager
		{
		private:
			std::vector<std::unique_ptr<IRPass>> passes;

		public:
			IRPassManager() {}

			void AddPass(std::unique_ptr<IRPass> pass)
			{
				passes.push_back(std::move(pass));
			}

			bool Run(IRModule* module)
			{
				bool changed = false;

				for (auto& pass : passes)
				{
					if (pass->RunOnModule(module))
					{
						changed = true;
					}
				}

				return changed;
			}

			void Clear()
			{
				passes.clear();
			}
		};
	} // namespace ir
} // namespace mdsl
