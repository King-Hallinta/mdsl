#pragma once

#include "../core/SourceLocation.h"

#include <cassert>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace mdsl
{
	namespace semantic
	{
		enum class SymbolKind
		{
			Variable,
			Function,
			Type,
			Constant,
			Namespace,
			UserDefined
		};

		struct Symbol
		{
			std::string		 name;
			SymbolKind		 kind;
			void*			 data;
			core::SourceSpan location;

			Symbol() : kind(SymbolKind::Variable), data(nullptr) {}

			Symbol(const std::string&	n,
				SymbolKind				k,
				void*					d = nullptr,
				const core::SourceSpan& loc = core::SourceSpan())
				: name(n), kind(k), data(d), location(loc)
			{
			}
		};

		class Scope
		{
		private:
			Scope*									parent;
			std::unordered_map<std::string, Symbol> symbols;
			std::vector<std::unique_ptr<Scope>>		children;

		public:
			Scope(Scope* parentScope = nullptr) : parent(parentScope) {}

			Scope* GetParent()
			{
				return parent;
			}

			const Scope* GetParent() const
			{
				return parent;
			}

			bool Define(const Symbol& symbol)
			{
				if (symbols.find(symbol.name) != symbols.end())
				{
					return false;
				}

				symbols[symbol.name] = symbol;
				return true;
			}

			Symbol* Lookup(const std::string& name)
			{
				auto it = symbols.find(name);
				if (it != symbols.end())
				{
					return &it->second;
				}
				return nullptr;
			}

			const Symbol* Lookup(const std::string& name) const
			{
				auto it = symbols.find(name);
				if (it != symbols.end())
				{
					return &it->second;
				}
				return nullptr;
			}

			Symbol* Resolve(const std::string& name)
			{
				Symbol* sym = Lookup(name);
				if (sym)
				{
					return sym;
				}

				if (parent)
				{
					return parent->Resolve(name);
				}

				return nullptr;
			}

			const Symbol* Resolve(const std::string& name) const
			{
				const Symbol* sym = Lookup(name);
				if (sym)
				{
					return sym;
				}

				if (parent)
				{
					return parent->Resolve(name);
				}

				return nullptr;
			}

			Scope* CreateChildScope()
			{
				children.push_back(std::make_unique<Scope>(this));
				return children.back().get();
			}

			const std::unordered_map<std::string, Symbol>& GetSymbols() const
			{
				return symbols;
			}
		};

		class SymbolTable
		{
		private:
			std::unique_ptr<Scope> globalScope;
			Scope*				   currentScope;

		public:
			SymbolTable() : globalScope(std::make_unique<Scope>()), currentScope(globalScope.get()) {}

			Scope* GetGlobalScope()
			{
				return globalScope.get();
			}

			const Scope* GetGlobalScope() const
			{
				return globalScope.get();
			}

			Scope* GetCurrentScope()
			{
				return currentScope;
			}

			const Scope* GetCurrentScope() const
			{
				return currentScope;
			}

			void EnterScope()
			{
				currentScope = currentScope->CreateChildScope();
			}

			void ExitScope()
			{
				assert(currentScope->GetParent() && "Cannot exit global scope");
				if (currentScope->GetParent())
				{
					currentScope = currentScope->GetParent();
				}
			}

			bool Define(const Symbol& symbol)
			{
				return currentScope->Define(symbol);
			}

			Symbol* Lookup(const std::string& name)
			{
				return currentScope->Lookup(name);
			}

			const Symbol* Lookup(const std::string& name) const
			{
				return currentScope->Lookup(name);
			}

			Symbol* Resolve(const std::string& name)
			{
				return currentScope->Resolve(name);
			}

			const Symbol* Resolve(const std::string& name) const
			{
				return currentScope->Resolve(name);
			}

			void Reset()
			{
				globalScope = std::make_unique<Scope>();
				currentScope = globalScope.get();
			}
		};
	} // namespace semantic
} // namespace mdsl
