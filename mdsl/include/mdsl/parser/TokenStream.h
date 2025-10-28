#pragma once

#include "../lexer/ILexer.h"
#include "../lexer/Token.h"

#include <memory>
#include <vector>

namespace mdsl
{
	namespace parser
	{
		struct TokenStreamCheckpoint
		{
			size_t position;
		};

		class TokenStream
		{
		private:
			std::vector<lexer::Token> tokens;
			size_t					  position;

		public:
			TokenStream() : position(0) {}

			explicit TokenStream(const std::vector<lexer::Token>& toks) : tokens(toks), position(0) {}

			explicit TokenStream(std::shared_ptr<lexer::ILexer> lexer) : position(0)
			{
				while (true)
				{
					auto tok = lexer->NextToken();
					tokens.push_back(tok);
					if (tok.type == lexer::TokenType::EndOfFile)
					{
						break;
					}
				}
			}

			lexer::Token NextToken()
			{
				if (position >= tokens.size())
				{
					if (tokens.empty() || tokens.back().type != lexer::TokenType::EndOfFile)
					{
						return lexer::Token(lexer::TokenType::EndOfFile, "", core::SourceSpan());
					}
					return tokens.back();
				}
				return tokens[position++];
			}

			lexer::Token PeekToken(size_t lookahead = 0)
			{
				size_t idx = position + lookahead;
				if (idx >= tokens.size())
				{
					if (tokens.empty() || tokens.back().type != lexer::TokenType::EndOfFile)
					{
						return lexer::Token(lexer::TokenType::EndOfFile, "", core::SourceSpan());
					}
					return tokens.back();
				}
				return tokens[idx];
			}

			bool IsAtEnd() const
			{
				if (position >= tokens.size())
				{
					return true;
				}
				return tokens[position].type == lexer::TokenType::EndOfFile;
			}

			TokenStreamCheckpoint SaveCheckpoint() const
			{
				TokenStreamCheckpoint checkpoint;
				checkpoint.position = position;
				return checkpoint;
			}

			void RestoreCheckpoint(const TokenStreamCheckpoint& checkpoint)
			{
				position = checkpoint.position;
			}

			void Reset()
			{
				position = 0;
			}

			size_t GetPosition() const
			{
				return position;
			}

			const std::vector<lexer::Token>& GetTokens() const
			{
				return tokens;
			}

			size_t Size() const
			{
				return tokens.size();
			}
		};
	} // namespace parser
} // namespace mdsl
