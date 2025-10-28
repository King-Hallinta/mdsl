#pragma once

#include "../core/StringPool.h"
#include "ILexer.h"

#include <functional>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace mdsl
{
	namespace lexer
	{
		struct LexerConfig
		{
			bool		skipWhitespace;
			bool		skipComments;
			std::string lineCommentStart;
			std::string blockCommentStart;
			std::string blockCommentEnd;
			bool		caseInsensitiveKeywords;
			std::string identifierStartChars;
			std::string identifierChars;

			LexerConfig()
				: skipWhitespace(true), skipComments(true), lineCommentStart("//"), blockCommentStart("/*"),
				  blockCommentEnd("*/"), caseInsensitiveKeywords(false), identifierStartChars(""),
				  identifierChars("")
			{
			}
		};

		class Lexer : public ILexer
		{
		private:
			std::string_view						   source;
			const char*								   filename;
			size_t									   position;
			size_t									   line;
			size_t									   column;
			std::shared_ptr<core::DiagnosticEngine>	   diagnostics;
			std::shared_ptr<core::StringPool>		   stringPool;
			LexerConfig								   config;
			std::unordered_map<std::string, TokenType> keywords;
			std::vector<Token>						   lookaheadBuffer;

			char currentChar() const
			{
				if (position >= source.length())
				{
					return '\0';
				}
				return source[position];
			}

			char peekChar(size_t offset = 1) const
			{
				size_t pos = position + offset;
				if (pos >= source.length())
				{
					return '\0';
				}
				return source[pos];
			}

			void Advance()
			{
				if (position < source.length())
				{
					if (source[position] == '\n')
					{
						line++;
						column = 1;
					}
					else
					{
						column++;
					}
					position++;
				}
			}

			void skipWhitespace()
			{
				while (std::isspace(currentChar()))
				{
					Advance();
				}
			}

			bool skipLineComment()
			{
				if (config.lineCommentStart.empty())
				{
					return false;
				}

				size_t commentLen = config.lineCommentStart.length();
				if (position + commentLen > source.length())
				{
					return false;
				}

				if (source.substr(position, commentLen) == config.lineCommentStart)
				{
					while (currentChar() != '\n' && currentChar() != '\0')
					{
						Advance();
					}
					return true;
				}
				return false;
			}

			bool skipBlockComment()
			{
				if (config.blockCommentStart.empty() || config.blockCommentEnd.empty())
				{
					return false;
				}

				size_t startLen = config.blockCommentStart.length();
				if (position + startLen > source.length())
				{
					return false;
				}

				if (source.substr(position, startLen) == config.blockCommentStart)
				{
					position += startLen;
					column += startLen;

					size_t endLen = config.blockCommentEnd.length();
					while (currentChar() != '\0')
					{
						if (position + endLen <= source.length() &&
							source.substr(position, endLen) == config.blockCommentEnd)
						{
							position += endLen;
							column += endLen;
							return true;
						}
						Advance();
					}

					if (diagnostics)
					{
						diagnostics->ReportError("Unterminated block comment");
					}
					return true;
				}
				return false;
			}

			void skipWhitespaceAndComments()
			{
				while (true)
				{
					if (config.skipWhitespace && std::isspace(currentChar()))
					{
						skipWhitespace();
						continue;
					}

					if (config.skipComments)
					{
						if (skipLineComment() || skipBlockComment())
						{
							continue;
						}
					}

					break;
				}
			}

			core::SourceLocation GetCurrentLocation() const
			{
				return core::SourceLocation(
					filename, static_cast<uint32_t>(line), static_cast<uint32_t>(column));
			}

			bool isIdentifierStart(char ch) const
			{
				return std::isalpha(ch) || ch == '_' ||
					   config.identifierStartChars.find(ch) != std::string::npos;
			}

			bool isIdentifierChar(char ch) const
			{
				return std::isalnum(ch) || ch == '_' || config.identifierChars.find(ch) != std::string::npos;
			}

			std::string toLower(const std::string& str) const
			{
				std::string result = str;
				for (char& c : result)
				{
					c = std::tolower(static_cast<unsigned char>(c));
				}
				return result;
			}

			Token lexIdentifier()
			{
				core::SourceLocation start = GetCurrentLocation();
				size_t				 startPos = position;

				while (isIdentifierChar(currentChar()))
				{
					Advance();
				}

				std::string lexeme(source.substr(startPos, position - startPos));

				TokenType	type = TokenType::Identifier;
				std::string lookupKey = config.caseInsensitiveKeywords ? toLower(lexeme) : lexeme;

				auto it = keywords.find(lookupKey);
				if (it != keywords.end())
				{
					type = it->second;
				}

				return Token(type, lexeme, core::SourceSpan(start, GetCurrentLocation()));
			}

			Token lexNumber()
			{
				core::SourceLocation start = GetCurrentLocation();
				size_t				 startPos = position;
				bool				 isFloat = false;

				while (std::isdigit(currentChar()))
				{
					Advance();
				}

				if (currentChar() == '.' && std::isdigit(peekChar()))
				{
					isFloat = true;
					Advance();

					while (std::isdigit(currentChar()))
					{
						Advance();
					}
				}

				if (currentChar() == 'e' || currentChar() == 'E')
				{
					isFloat = true;
					Advance();

					if (currentChar() == '+' || currentChar() == '-')
					{
						Advance();
					}

					while (std::isdigit(currentChar()))
					{
						Advance();
					}
				}

				std::string lexeme(source.substr(startPos, position - startPos));
				Token		token(isFloat ? TokenType::Float : TokenType::Integer,
					lexeme,
					core::SourceSpan(start, GetCurrentLocation()));

				if (isFloat)
				{
					try
					{
						token.floatValue = std::stod(lexeme);
					}
					catch (const std::out_of_range&)
					{
						if (diagnostics)
						{
							diagnostics->ReportError("Floating point number out of range: " + lexeme,
								core::SourceSpan(start, GetCurrentLocation()));
						}
						token.type = TokenType::Error;
					}
					catch (const std::invalid_argument&)
					{
						if (diagnostics)
						{
							diagnostics->ReportError("Invalid floating point number: " + lexeme,
								core::SourceSpan(start, GetCurrentLocation()));
						}
						token.type = TokenType::Error;
					}
				}
				else
				{
					try
					{
						token.intValue = std::stoll(lexeme);
					}
					catch (const std::out_of_range&)
					{
						if (diagnostics)
						{
							diagnostics->ReportError("Integer out of range: " + lexeme,
								core::SourceSpan(start, GetCurrentLocation()));
						}
						token.type = TokenType::Error;
					}
					catch (const std::invalid_argument&)
					{
						if (diagnostics)
						{
							diagnostics->ReportError(
								"Invalid integer: " + lexeme, core::SourceSpan(start, GetCurrentLocation()));
						}
						token.type = TokenType::Error;
					}
				}

				return token;
			}

			Token lexString()
			{
				core::SourceLocation start = GetCurrentLocation();
				char				 quote = currentChar();
				Advance();

				std::string value;

				while (currentChar() != quote && currentChar() != '\0')
				{
					if (currentChar() == '\\')
					{
						Advance();
						switch (currentChar())
						{
							case 'n':
								value += '\n';
								break;
							case 't':
								value += '\t';
								break;
							case 'r':
								value += '\r';
								break;
							case '\\':
								value += '\\';
								break;
							case '"':
								value += '"';
								break;
							case '\'':
								value += '\'';
								break;
							default:
								value += currentChar();
								break;
						}
						Advance();
					}
					else
					{
						value += currentChar();
						Advance();
					}
				}

				if (currentChar() == '\0')
				{
					if (diagnostics)
					{
						diagnostics->ReportError(
							"Unterminated string literal", core::SourceSpan(start, GetCurrentLocation()));
					}
				}
				else
				{
					Advance();
				}

				return Token(TokenType::String, value, core::SourceSpan(start, GetCurrentLocation()));
			}

			Token lexSingleCharToken(TokenType type)
			{
				core::SourceLocation start = GetCurrentLocation();
				char				 ch = currentChar();
				Advance();
				return Token(type, std::string(1, ch), core::SourceSpan(start, GetCurrentLocation()));
			}

			Token lexNextToken()
			{
				skipWhitespaceAndComments();

				if (currentChar() == '\0')
				{
					return Token(TokenType::EndOfFile,
						"",
						core::SourceSpan(GetCurrentLocation(), GetCurrentLocation()));
				}

				char ch = currentChar();

				if (isIdentifierStart(ch))
				{
					return lexIdentifier();
				}

				if (std::isdigit(ch))
				{
					return lexNumber();
				}

				if (ch == '"' || ch == '\'')
				{
					return lexString();
				}

				core::SourceLocation start = GetCurrentLocation();

				switch (ch)
				{
					case '(':
						return lexSingleCharToken(TokenType::LeftParen);
					case ')':
						return lexSingleCharToken(TokenType::RightParen);
					case '{':
						return lexSingleCharToken(TokenType::LeftBrace);
					case '}':
						return lexSingleCharToken(TokenType::RightBrace);
					case '[':
						return lexSingleCharToken(TokenType::LeftBracket);
					case ']':
						return lexSingleCharToken(TokenType::RightBracket);
					case ',':
						return lexSingleCharToken(TokenType::Comma);
					case '.':
						return lexSingleCharToken(TokenType::Dot);
					case ';':
						return lexSingleCharToken(TokenType::Semicolon);
					case ':':
						return lexSingleCharToken(TokenType::Colon);
					case '+':
						Advance();
						if (currentChar() == '=')
						{
							Advance();
							return Token(
								TokenType::PlusEqual, "+=", core::SourceSpan(start, GetCurrentLocation()));
						}
						return Token(TokenType::Plus, "+", core::SourceSpan(start, GetCurrentLocation()));
					case '-':
						Advance();
						if (currentChar() == '=')
						{
							Advance();
							return Token(
								TokenType::MinusEqual, "-=", core::SourceSpan(start, GetCurrentLocation()));
						}
						if (currentChar() == '>')
						{
							Advance();
							return Token(
								TokenType::Arrow, "->", core::SourceSpan(start, GetCurrentLocation()));
						}
						return Token(TokenType::Minus, "-", core::SourceSpan(start, GetCurrentLocation()));
					case '*':
						Advance();
						if (currentChar() == '=')
						{
							Advance();
							return Token(
								TokenType::StarEqual, "*=", core::SourceSpan(start, GetCurrentLocation()));
						}
						return Token(TokenType::Star, "*", core::SourceSpan(start, GetCurrentLocation()));
					case '/':
						Advance();
						if (currentChar() == '=')
						{
							Advance();
							return Token(
								TokenType::SlashEqual, "/=", core::SourceSpan(start, GetCurrentLocation()));
						}
						return Token(TokenType::Slash, "/", core::SourceSpan(start, GetCurrentLocation()));
					case '%':
						return lexSingleCharToken(TokenType::Percent);
					case '=':
						Advance();
						if (currentChar() == '=')
						{
							Advance();
							return Token(
								TokenType::EqualEqual, "==", core::SourceSpan(start, GetCurrentLocation()));
						}
						return Token(TokenType::Equal, "=", core::SourceSpan(start, GetCurrentLocation()));
					case '!':
						Advance();
						if (currentChar() == '=')
						{
							Advance();
							return Token(
								TokenType::BangEqual, "!=", core::SourceSpan(start, GetCurrentLocation()));
						}
						return Token(TokenType::Bang, "!", core::SourceSpan(start, GetCurrentLocation()));
					case '<':
						Advance();
						if (currentChar() == '=')
						{
							Advance();
							return Token(
								TokenType::LessEqual, "<=", core::SourceSpan(start, GetCurrentLocation()));
						}
						if (currentChar() == '<')
						{
							Advance();
							return Token(
								TokenType::LeftShift, "<<", core::SourceSpan(start, GetCurrentLocation()));
						}
						return Token(TokenType::Less, "<", core::SourceSpan(start, GetCurrentLocation()));
					case '>':
						Advance();
						if (currentChar() == '=')
						{
							Advance();
							return Token(
								TokenType::GreaterEqual, ">=", core::SourceSpan(start, GetCurrentLocation()));
						}
						if (currentChar() == '>')
						{
							Advance();
							return Token(
								TokenType::RightShift, ">>", core::SourceSpan(start, GetCurrentLocation()));
						}
						return Token(TokenType::Greater, ">", core::SourceSpan(start, GetCurrentLocation()));
					case '&':
						Advance();
						if (currentChar() == '&')
						{
							Advance();
							return Token(TokenType::AmpersandAmpersand,
								"&&",
								core::SourceSpan(start, GetCurrentLocation()));
						}
						return Token(
							TokenType::Ampersand, "&", core::SourceSpan(start, GetCurrentLocation()));
					case '|':
						Advance();
						if (currentChar() == '|')
						{
							Advance();
							return Token(
								TokenType::PipePipe, "||", core::SourceSpan(start, GetCurrentLocation()));
						}
						return Token(TokenType::Pipe, "|", core::SourceSpan(start, GetCurrentLocation()));
					case '^':
						return lexSingleCharToken(TokenType::Caret);
					case '~':
						return lexSingleCharToken(TokenType::Tilde);
					default:
						break;
				}

				if (diagnostics)
				{
					diagnostics->ReportError("Unexpected character: " + std::string(1, ch),
						core::SourceSpan(start, GetCurrentLocation()));
				}

				Advance();
				return Token(
					TokenType::Error, std::string(1, ch), core::SourceSpan(start, GetCurrentLocation()));
			}

		public:
			Lexer(std::string_view src, const char* file = "<input>")
				: source(src), filename(file), position(0), line(1), column(1),
				  stringPool(std::make_shared<core::StringPool>())
			{
			}

			void SetConfig(const LexerConfig& cfg)
			{
				config = cfg;
			}

			void AddKeyword(const std::string& keyword, TokenType type)
			{
				std::string key = config.caseInsensitiveKeywords ? toLower(keyword) : keyword;
				keywords[key] = type;
			}

			Token NextToken() override
			{
				if (!lookaheadBuffer.empty())
				{
					Token token = lookaheadBuffer.front();
					lookaheadBuffer.erase(lookaheadBuffer.begin());
					return token;
				}

				return lexNextToken();
			}

			Token PeekToken(size_t lookahead = 0) override
			{
				while (lookaheadBuffer.size() <= lookahead)
				{
					lookaheadBuffer.push_back(lexNextToken());
				}

				return lookaheadBuffer[lookahead];
			}

			void Reset() override
			{
				position = 0;
				line = 1;
				column = 1;
				lookaheadBuffer.clear();
			}

			bool IsAtEnd() const override
			{
				return position >= source.length();
			}

			void SetDiagnosticEngine(std::shared_ptr<core::DiagnosticEngine> engine) override
			{
				diagnostics = engine;
			}
		};
	} // namespace lexer
} // namespace mdsl
