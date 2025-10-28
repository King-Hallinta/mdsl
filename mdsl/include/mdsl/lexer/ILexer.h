#pragma once

#include "../core/Diagnostic.h"
#include "Token.h"

#include <memory>

namespace mdsl
{
	namespace lexer
	{
		class LexerCheckpoint
		{
		public:
			size_t				position;
			size_t				line;
			size_t				column;
			std::vector<Token>	lookaheadBuffer;
		};

		class ILexer
		{
		public:
			virtual ~ILexer() = default;

			virtual Token NextToken() = 0;
			virtual Token PeekToken(size_t lookahead = 0) = 0;
			virtual void  Reset() = 0;
			virtual bool  IsAtEnd() const = 0;

			virtual LexerCheckpoint SaveCheckpoint() = 0;
			virtual void			RestoreCheckpoint(const LexerCheckpoint& checkpoint) = 0;

			virtual void SetDiagnosticEngine(std::shared_ptr<core::DiagnosticEngine> engine) = 0;
		};
	} // namespace lexer
} // namespace mdsl
