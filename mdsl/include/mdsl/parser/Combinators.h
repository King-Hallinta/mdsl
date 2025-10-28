#pragma once

#include "../core/Result.h"
#include "../lexer/ILexer.h"
#include "ASTNode.h"
#include "TokenStream.h"

#include <functional>
#include <memory>
#include <vector>

namespace mdsl
{
	namespace parser
	{
		template <typename T> using ParseResult = core::Result<T>;

		template <typename T> class Parser
		{
		public:
			using ParseFunc = std::function<ParseResult<T>(std::shared_ptr<lexer::ILexer>&)>;

		private:
			ParseFunc parseFunc;

		public:
			Parser(ParseFunc func) : parseFunc(func) {}

			ParseResult<T> Parse(std::shared_ptr<lexer::ILexer>& lexer)
			{
				return parseFunc(lexer);
			}

			template <typename U> Parser<U> map(std::function<U(const T&)> mapper)
			{
				return Parser<U>(
					[this, mapper](std::shared_ptr<lexer::ILexer>& lexer) -> ParseResult<U>
					{
						auto result = this->Parse(lexer);
						if (result.IsError())
						{
							return ParseResult<U>(result.GetError());
						}
						return ParseResult<U>(mapper(result.Value()));
					});
			}

			template <typename U> Parser<U> flatMap(std::function<Parser<U>(const T&)> mapper)
			{
				return Parser<U>(
					[this, mapper](std::shared_ptr<lexer::ILexer>& lexer) -> ParseResult<U>
					{
						auto result = this->Parse(lexer);
						if (result.IsError())
						{
							return ParseResult<U>(result.GetError());
						}
						return mapper(result.Value()).Parse(lexer);
					});
			}

			Parser<T> orElse(Parser<T> alternative)
			{
				return Parser<T>(
					[this, alternative](std::shared_ptr<lexer::ILexer>& lexer) -> ParseResult<T>
					{
						auto checkpoint = lexer->SaveCheckpoint();
						auto result = this->Parse(lexer);
						if (result.IsOk())
						{
							return result;
						}
						lexer->RestoreCheckpoint(checkpoint);
						return alternative.Parse(lexer);
					});
			}
		};

		template <typename T> Parser<std::vector<T>> many(Parser<T> parser)
		{
			return Parser<std::vector<T>>(
				[parser](std::shared_ptr<lexer::ILexer>& lexer) -> ParseResult<std::vector<T>>
				{
					std::vector<T> results;

					while (true)
					{
						auto checkpoint = lexer->SaveCheckpoint();
						auto result = parser.Parse(lexer);
						if (result.IsError())
						{
							lexer->RestoreCheckpoint(checkpoint);
							break;
						}
						results.push_back(result.Value());
					}

					return ParseResult<std::vector<T>>(results);
				});
		}

		template <typename T> Parser<std::vector<T>> many1(Parser<T> parser)
		{
			return Parser<std::vector<T>>(
				[parser](std::shared_ptr<lexer::ILexer>& lexer) -> ParseResult<std::vector<T>>
				{
					std::vector<T> results;

					auto first = parser.Parse(lexer);
					if (first.IsError())
					{
						return ParseResult<std::vector<T>>(first.GetError());
					}
					results.push_back(first.Value());

					while (true)
					{
						auto checkpoint = lexer->SaveCheckpoint();
						auto result = parser.Parse(lexer);
						if (result.IsError())
						{
							lexer->RestoreCheckpoint(checkpoint);
							break;
						}
						results.push_back(result.Value());
					}

					return ParseResult<std::vector<T>>(results);
				});
		}

		template <typename T> Parser<T> optional(Parser<T> parser, T defaultValue)
		{
			return Parser<T>(
				[parser, defaultValue](std::shared_ptr<lexer::ILexer>& lexer) -> ParseResult<T>
				{
					auto checkpoint = lexer->SaveCheckpoint();
					auto result = parser.Parse(lexer);
					if (result.IsOk())
					{
						return result;
					}
					lexer->RestoreCheckpoint(checkpoint);
					return ParseResult<T>(defaultValue);
				});
		}

		template <typename T, typename Sep>
		Parser<std::vector<T>> sepBy(Parser<T> parser, Parser<Sep> separator)
		{
			return Parser<std::vector<T>>(
				[parser, separator](std::shared_ptr<lexer::ILexer>& lexer) -> ParseResult<std::vector<T>>
				{
					std::vector<T> results;

					auto checkpoint = lexer->SaveCheckpoint();
					auto first = parser.Parse(lexer);
					if (first.IsError())
					{
						lexer->RestoreCheckpoint(checkpoint);
						return ParseResult<std::vector<T>>(results);
					}
					results.push_back(first.Value());

					while (true)
					{
						checkpoint = lexer->SaveCheckpoint();
						auto sepResult = separator.Parse(lexer);
						if (sepResult.IsError())
						{
							lexer->RestoreCheckpoint(checkpoint);
							break;
						}

						auto itemResult = parser.Parse(lexer);
						if (itemResult.IsError())
						{
							lexer->RestoreCheckpoint(checkpoint);
							return ParseResult<std::vector<T>>(itemResult.GetError());
						}
						results.push_back(itemResult.Value());
					}

					return ParseResult<std::vector<T>>(results);
				});
		}

		inline Parser<lexer::Token> token(lexer::TokenType type)
		{
			return Parser<lexer::Token>(
				[type](std::shared_ptr<lexer::ILexer>& lexer) -> ParseResult<lexer::Token>
				{
					auto tok = lexer->NextToken();
					if (tok.type == type)
					{
						return ParseResult<lexer::Token>(tok);
					}

					return ParseResult<lexer::Token>(
						core::Error(core::ErrorCode::ParserError, "Unexpected token"));
				});
		}

		template <typename T> class StreamParser
		{
		public:
			using ParseFunc = std::function<ParseResult<T>(TokenStream&)>;

		private:
			ParseFunc parseFunc;

		public:
			StreamParser(ParseFunc func) : parseFunc(func) {}

			ParseResult<T> Parse(TokenStream& stream) const
			{
				return parseFunc(stream);
			}

			template <typename U> StreamParser<U> map(std::function<U(const T&)> mapper) const
			{
				return StreamParser<U>(
					[this, mapper](TokenStream& stream) -> ParseResult<U>
					{
						auto result = this->Parse(stream);
						if (result.IsError())
						{
							return ParseResult<U>(result.GetError());
						}
						return ParseResult<U>(mapper(result.Value()));
					});
			}

			template <typename U> StreamParser<U> flatMap(std::function<StreamParser<U>(const T&)> mapper) const
			{
				return StreamParser<U>(
					[this, mapper](TokenStream& stream) -> ParseResult<U>
					{
						auto result = this->Parse(stream);
						if (result.IsError())
						{
							return ParseResult<U>(result.GetError());
						}
						return mapper(result.Value()).Parse(stream);
					});
			}

			StreamParser<T> orElse(StreamParser<T> alternative) const
			{
				return StreamParser<T>(
					[this, alternative](TokenStream& stream) -> ParseResult<T>
					{
						auto checkpoint = stream.SaveCheckpoint();
						auto result = this->Parse(stream);
						if (result.IsOk())
						{
							return result;
						}
						stream.RestoreCheckpoint(checkpoint);
						return alternative.Parse(stream);
					});
			}
		};

		template <typename T> StreamParser<std::vector<T>> streamMany(StreamParser<T> parser)
		{
			return StreamParser<std::vector<T>>(
				[parser](TokenStream& stream) -> ParseResult<std::vector<T>>
				{
					std::vector<T> results;

					while (true)
					{
						auto checkpoint = stream.SaveCheckpoint();
						auto result = parser.Parse(stream);
						if (result.IsError())
						{
							stream.RestoreCheckpoint(checkpoint);
							break;
						}
						results.push_back(result.Value());
					}

					return ParseResult<std::vector<T>>(results);
				});
		}

		template <typename T> StreamParser<std::vector<T>> streamMany1(StreamParser<T> parser)
		{
			return StreamParser<std::vector<T>>(
				[parser](TokenStream& stream) -> ParseResult<std::vector<T>>
				{
					std::vector<T> results;

					auto first = parser.Parse(stream);
					if (first.IsError())
					{
						return ParseResult<std::vector<T>>(first.GetError());
					}
					results.push_back(first.Value());

					while (true)
					{
						auto checkpoint = stream.SaveCheckpoint();
						auto result = parser.Parse(stream);
						if (result.IsError())
						{
							stream.RestoreCheckpoint(checkpoint);
							break;
						}
						results.push_back(result.Value());
					}

					return ParseResult<std::vector<T>>(results);
				});
		}

		template <typename T> StreamParser<T> streamOptional(StreamParser<T> parser, T defaultValue)
		{
			return StreamParser<T>(
				[parser, defaultValue](TokenStream& stream) -> ParseResult<T>
				{
					auto checkpoint = stream.SaveCheckpoint();
					auto result = parser.Parse(stream);
					if (result.IsOk())
					{
						return result;
					}
					stream.RestoreCheckpoint(checkpoint);
					return ParseResult<T>(defaultValue);
				});
		}

		template <typename T, typename Sep>
		StreamParser<std::vector<T>> streamSepBy(StreamParser<T> parser, StreamParser<Sep> separator)
		{
			return StreamParser<std::vector<T>>(
				[parser, separator](TokenStream& stream) -> ParseResult<std::vector<T>>
				{
					std::vector<T> results;

					auto checkpoint = stream.SaveCheckpoint();
					auto first = parser.Parse(stream);
					if (first.IsError())
					{
						stream.RestoreCheckpoint(checkpoint);
						return ParseResult<std::vector<T>>(results);
					}
					results.push_back(first.Value());

					while (true)
					{
						checkpoint = stream.SaveCheckpoint();
						auto sepResult = separator.Parse(stream);
						if (sepResult.IsError())
						{
							stream.RestoreCheckpoint(checkpoint);
							break;
						}

						auto itemResult = parser.Parse(stream);
						if (itemResult.IsError())
						{
							stream.RestoreCheckpoint(checkpoint);
							return ParseResult<std::vector<T>>(itemResult.GetError());
						}
						results.push_back(itemResult.Value());
					}

					return ParseResult<std::vector<T>>(results);
				});
		}

		inline StreamParser<lexer::Token> streamToken(lexer::TokenType type)
		{
			return StreamParser<lexer::Token>(
				[type](TokenStream& stream) -> ParseResult<lexer::Token>
				{
					auto tok = stream.NextToken();
					if (tok.type == type)
					{
						return ParseResult<lexer::Token>(tok);
					}

					return ParseResult<lexer::Token>(
						core::Error(core::ErrorCode::ParserError, "Unexpected token"));
				});
		}
	} // namespace parser
} // namespace mdsl
