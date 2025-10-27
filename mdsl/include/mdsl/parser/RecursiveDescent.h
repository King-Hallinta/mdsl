#pragma once

#include "../lexer/Token.h"
#include "IParser.h"

#include <functional>

namespace mdsl
{
	namespace parser
	{
		class RecursiveDescentParser : public IParser
		{
		protected:
			std::shared_ptr<lexer::ILexer>			lexer;
			std::shared_ptr<core::DiagnosticEngine> diagnostics;
			lexer::Token							currentToken;

			void Advance()
			{
				if (lexer)
				{
					currentToken = lexer->NextToken();
				}
			}

			const lexer::Token& peek(size_t lookahead = 0)
			{
				if (lookahead == 0)
				{
					return currentToken;
				}

				if (lexer)
				{
					return lexer->PeekToken(lookahead - 1);
				}

				static lexer::Token eofToken(lexer::TokenType::EndOfFile, "", core::SourceSpan());
				return eofToken;
			}

			bool Check(lexer::TokenType type)
			{
				return currentToken.type == type;
			}

			bool Match(lexer::TokenType type)
			{
				if (Check(type))
				{
					Advance();
					return true;
				}
				return false;
			}

			template <typename... Args> bool Match(lexer::TokenType first, Args... rest)
			{
				if (Match(first))
				{
					return true;
				}
				return Match(rest...);
			}

			bool Expect(lexer::TokenType type, const std::string& errorMessage)
			{
				if (Check(type))
				{
					Advance();
					return true;
				}

				if (diagnostics)
				{
					diagnostics->ReportError(errorMessage, currentToken.location);
				}
				return false;
			}

			void Synchronize(std::function<bool(const lexer::Token&)> predicate)
			{
				while (!Check(lexer::TokenType::EndOfFile))
				{
					if (predicate(currentToken))
					{
						break;
					}
					Advance();
				}
			}

			bool IsAtEnd() const
			{
				return currentToken.type == lexer::TokenType::EndOfFile;
			}

		public:
			RecursiveDescentParser() {}

			void SetLexer(std::shared_ptr<lexer::ILexer> lex) override
			{
				lexer = lex;
				if (lexer)
				{
					currentToken = lexer->NextToken();
				}
			}

			void SetDiagnosticEngine(std::shared_ptr<core::DiagnosticEngine> engine) override
			{
				diagnostics = engine;
			}

			void Reset() override
			{
				if (lexer)
				{
					lexer->Reset();
					currentToken = lexer->NextToken();
				}
			}

			std::unique_ptr<ASTNode> Parse() override
			{
				return nullptr;
			}
		};
	} // namespace parser
} // namespace mdsl
