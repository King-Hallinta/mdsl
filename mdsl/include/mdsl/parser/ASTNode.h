#pragma once

#include "../core/SourceLocation.h"

#include <memory>
#include <string>
#include <vector>

namespace mdsl
{
	namespace parser
	{
		enum class ASTNodeType
		{
			Unknown,
			Expression,
			Statement,
			Declaration,
			TypeReference,
			UserDefinedStart = 1000
		};

		class ASTNode
		{
		protected:
			ASTNodeType		 nodeType;
			core::SourceSpan location;

		public:
			ASTNode(ASTNodeType type, const core::SourceSpan& loc) : nodeType(type), location(loc) {}

			virtual ~ASTNode() = default;

			ASTNodeType GetNodeType() const
			{
				return nodeType;
			}

			const core::SourceSpan& GetLocation() const
			{
				return location;
			}

			template <typename T> bool is() const
			{
				return dynamic_cast<const T*>(this) != nullptr;
			}

			template <typename T> T* as()
			{
				return dynamic_cast<T*>(this);
			}

			template <typename T> const T* as() const
			{
				return dynamic_cast<const T*>(this);
			}
		};

		class Expression : public ASTNode
		{
		public:
			Expression(ASTNodeType type, const core::SourceSpan& loc) : ASTNode(type, loc) {}
		};

		class Statement : public ASTNode
		{
		public:
			Statement(ASTNodeType type, const core::SourceSpan& loc) : ASTNode(type, loc) {}
		};

		class Declaration : public ASTNode
		{
		public:
			Declaration(ASTNodeType type, const core::SourceSpan& loc) : ASTNode(type, loc) {}
		};

		template <typename ReturnType> class ASTVisitor
		{
		public:
			virtual ~ASTVisitor() = default;

			virtual ReturnType Visit(ASTNode* node) = 0;
			virtual ReturnType Visit(Expression* node) = 0;
			virtual ReturnType Visit(Statement* node) = 0;
			virtual ReturnType Visit(Declaration* node) = 0;
		};
	} // namespace parser
} // namespace mdsl
