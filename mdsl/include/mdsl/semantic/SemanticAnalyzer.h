#pragma once

#include "../core/Diagnostic.h"
#include "../parser/ASTNode.h"
#include "SymbolTable.h"
#include "Type.h"

#include <memory>

namespace mdsl
{
	namespace semantic
	{
		class SemanticAnalyzer
		{
		protected:
			std::shared_ptr<SymbolTable>			symbolTable;
			std::shared_ptr<TypeRegistry>			typeRegistry;
			std::shared_ptr<core::DiagnosticEngine> diagnostics;

		public:
			SemanticAnalyzer()
				: symbolTable(std::make_shared<SymbolTable>()), typeRegistry(std::make_shared<TypeRegistry>())
			{
			}

			void SetSymbolTable(std::shared_ptr<SymbolTable> table)
			{
				symbolTable = table;
			}

			void SetTypeRegistry(std::shared_ptr<TypeRegistry> registry)
			{
				typeRegistry = registry;
			}

			void SetDiagnosticEngine(std::shared_ptr<core::DiagnosticEngine> engine)
			{
				diagnostics = engine;
			}

			std::shared_ptr<SymbolTable> GetSymbolTable() const
			{
				return symbolTable;
			}

			std::shared_ptr<TypeRegistry> GetTypeRegistry() const
			{
				return typeRegistry;
			}

			virtual bool Analyze(parser::ASTNode* root)
			{
				return false;
			}

			virtual void Reset()
			{
				if (symbolTable)
				{
					symbolTable->Reset();
				}
				if (typeRegistry)
				{
					typeRegistry->Clear();
				}
			}

		protected:
			void ReportError(const std::string& message, const core::SourceSpan& location)
			{
				if (diagnostics)
				{
					diagnostics->ReportError(message, location);
				}
			}

			void ReportWarning(const std::string& message, const core::SourceSpan& location)
			{
				if (diagnostics)
				{
					diagnostics->ReportWarning(message, location);
				}
			}
		};
	} // namespace semantic
} // namespace mdsl
