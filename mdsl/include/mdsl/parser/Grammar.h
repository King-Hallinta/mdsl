#pragma once

#include "../lexer/Token.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace mdsl
{
	namespace parser
	{
		enum class GrammarSymbolType
		{
			Terminal,
			NonTerminal
		};

		struct GrammarSymbol
		{
			GrammarSymbolType type;
			std::string		  name;
			lexer::TokenType  tokenType;

			GrammarSymbol() : type(GrammarSymbolType::NonTerminal), tokenType(lexer::TokenType::EndOfFile) {}

			static GrammarSymbol terminal(lexer::TokenType type, const std::string& name = "")
			{
				GrammarSymbol sym;
				sym.type = GrammarSymbolType::Terminal;
				sym.tokenType = type;
				sym.name = name.empty() ? std::to_string(static_cast<uint32_t>(type)) : name;
				return sym;
			}

			static GrammarSymbol nonTerminal(const std::string& name)
			{
				GrammarSymbol sym;
				sym.type = GrammarSymbolType::NonTerminal;
				sym.name = name;
				return sym;
			}

			bool operator==(const GrammarSymbol& other) const
			{
				return type == other.type && name == other.name &&
					   (type == GrammarSymbolType::NonTerminal || tokenType == other.tokenType);
			}
		};

		struct GrammarProduction
		{
			std::string				   nonTerminal;
			std::vector<GrammarSymbol> symbols;

			GrammarProduction() {}

			GrammarProduction(const std::string& nt, const std::vector<GrammarSymbol>& syms)
				: nonTerminal(nt), symbols(syms)
			{
			}
		};

		class Grammar
		{
		private:
			std::string														startSymbol;
			std::unordered_map<std::string, std::vector<GrammarProduction>> productions;

		public:
			Grammar() {}

			void SetStartSymbol(const std::string& symbol)
			{
				startSymbol = symbol;
			}

			const std::string& GetStartSymbol() const
			{
				return startSymbol;
			}

			void AddProduction(const GrammarProduction& production)
			{
				productions[production.nonTerminal].push_back(production);
			}

			void AddProduction(const std::string& nonTerminal, const std::vector<GrammarSymbol>& symbols)
			{
				productions[nonTerminal].push_back(GrammarProduction(nonTerminal, symbols));
			}

			const std::vector<GrammarProduction>& GetProductions(const std::string& nonTerminal) const
			{
				static std::vector<GrammarProduction> empty;
				auto								  it = productions.find(nonTerminal);
				if (it != productions.end())
				{
					return it->second;
				}
				return empty;
			}

			std::vector<std::string> GetNonTerminals() const
			{
				std::vector<std::string> result;
				for (const auto& pair : productions)
				{
					result.push_back(pair.first);
				}
				return result;
			}

			bool HasProduction(const std::string& nonTerminal) const
			{
				return productions.find(nonTerminal) != productions.end();
			}
		};
	} // namespace parser
} // namespace mdsl
