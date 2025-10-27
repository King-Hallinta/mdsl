#pragma once

#include "../core/Diagnostic.h"
#include "Token.h"

#include <memory>

namespace mdsl
{
	namespace lexer
	{
		class ILexer
		{
		public:
			virtual ~ILexer() = default;

			virtual Token NextToken() = 0;
			virtual Token PeekToken(size_t lookahead = 0) = 0;
			virtual void  Reset() = 0;
			virtual bool  IsAtEnd() const = 0;

			virtual void SetDiagnosticEngine(std::shared_ptr<core::DiagnosticEngine> engine) = 0;
		};
	} // namespace lexer
} // namespace mdsl
