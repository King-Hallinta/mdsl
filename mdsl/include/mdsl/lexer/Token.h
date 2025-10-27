#pragma once

#include "../core/SourceLocation.h"

#include <cstdint>
#include <string>

namespace mdsl
{
	namespace lexer
	{
		enum class TokenType : uint32_t
		{
			EndOfFile = 0,
			Error,

			Identifier,
			Integer,
			Float,
			String,
			Character,

			LeftParen,
			RightParen,
			LeftBrace,
			RightBrace,
			LeftBracket,
			RightBracket,

			Comma,
			Dot,
			Semicolon,
			Colon,
			Arrow,

			Plus,
			Minus,
			Star,
			Slash,
			Percent,

			Equal,
			EqualEqual,
			BangEqual,
			Less,
			LessEqual,
			Greater,
			GreaterEqual,

			Ampersand,
			Pipe,
			Caret,
			Tilde,
			Bang,

			AmpersandAmpersand,
			PipePipe,

			LeftShift,
			RightShift,

			PlusEqual,
			MinusEqual,
			StarEqual,
			SlashEqual,

			UserDefinedStart = 10000
		};

		struct Token
		{
			TokenType		 type;
			std::string		 lexeme;
			core::SourceSpan location;

			union
			{
				int64_t intValue;
				double	floatValue;
			};

			Token() : type(TokenType::EndOfFile), intValue(0) {}

			Token(TokenType t, const std::string& lex, const core::SourceSpan& loc)
				: type(t), lexeme(lex), location(loc), intValue(0)
			{
			}

			bool is(TokenType t) const
			{
				return type == t;
			}

			bool IsNot(TokenType t) const
			{
				return type != t;
			}

			bool IsOneOf(TokenType t1) const
			{
				return is(t1);
			}

			template <typename... Args> bool IsOneOf(TokenType t1, Args... args) const
			{
				return is(t1) || IsOneOf(args...);
			}
		};

		inline std::string TokenTypeToString(TokenType type)
		{
			switch (type)
			{
				case TokenType::EndOfFile:
					return "EndOfFile";
				case TokenType::Error:
					return "Error";
				case TokenType::Identifier:
					return "Identifier";
				case TokenType::Integer:
					return "Integer";
				case TokenType::Float:
					return "Float";
				case TokenType::String:
					return "String";
				case TokenType::Character:
					return "Character";
				case TokenType::LeftParen:
					return "LeftParen";
				case TokenType::RightParen:
					return "RightParen";
				case TokenType::LeftBrace:
					return "LeftBrace";
				case TokenType::RightBrace:
					return "RightBrace";
				case TokenType::LeftBracket:
					return "LeftBracket";
				case TokenType::RightBracket:
					return "RightBracket";
				case TokenType::Comma:
					return "Comma";
				case TokenType::Dot:
					return "Dot";
				case TokenType::Semicolon:
					return "Semicolon";
				case TokenType::Colon:
					return "Colon";
				case TokenType::Arrow:
					return "Arrow";
				case TokenType::Plus:
					return "Plus";
				case TokenType::Minus:
					return "Minus";
				case TokenType::Star:
					return "Star";
				case TokenType::Slash:
					return "Slash";
				case TokenType::Percent:
					return "Percent";
				case TokenType::Equal:
					return "Equal";
				case TokenType::EqualEqual:
					return "EqualEqual";
				case TokenType::BangEqual:
					return "BangEqual";
				case TokenType::Less:
					return "Less";
				case TokenType::LessEqual:
					return "LessEqual";
				case TokenType::Greater:
					return "Greater";
				case TokenType::GreaterEqual:
					return "GreaterEqual";
				case TokenType::Ampersand:
					return "Ampersand";
				case TokenType::Pipe:
					return "Pipe";
				case TokenType::Caret:
					return "Caret";
				case TokenType::Tilde:
					return "Tilde";
				case TokenType::Bang:
					return "Bang";
				case TokenType::AmpersandAmpersand:
					return "AmpersandAmpersand";
				case TokenType::PipePipe:
					return "PipePipe";
				case TokenType::LeftShift:
					return "LeftShift";
				case TokenType::RightShift:
					return "RightShift";
				case TokenType::PlusEqual:
					return "PlusEqual";
				case TokenType::MinusEqual:
					return "MinusEqual";
				case TokenType::StarEqual:
					return "StarEqual";
				case TokenType::SlashEqual:
					return "SlashEqual";
				default:
					return "Operator";
			}
		}
	} // namespace lexer
} // namespace mdsl
