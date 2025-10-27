#pragma once

#include "../core/Diagnostic.h"
#include "../lexer/ILexer.h"
#include "ASTNode.h"

#include <memory>

namespace mdsl
{
	namespace parser
	{
		class IParser
		{
		public:
			virtual ~IParser() = default;

			virtual std::unique_ptr<ASTNode> Parse() = 0;
			virtual void					 Reset() = 0;

			virtual void SetLexer(std::shared_ptr<lexer::ILexer> lexer) = 0;
			virtual void SetDiagnosticEngine(std::shared_ptr<core::DiagnosticEngine> engine) = 0;
		};
	} // namespace parser
} // namespace mdsl
