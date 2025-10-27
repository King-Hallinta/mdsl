#pragma once

#include <memory>
#include <string>
#include <vector>

namespace mdsl
{
	namespace semantic
	{
		enum class TypeKind
		{
			Void,
			Integer,
			Float,
			Boolean,
			String,
			Array,
			Pointer,
			Function,
			Struct,
			UserDefined
		};

		class Type
		{
		protected:
			TypeKind kind;

		public:
			Type(TypeKind k) : kind(k) {}

			virtual ~Type() = default;

			TypeKind GetKind() const
			{
				return kind;
			}

			virtual std::string ToString() const = 0;

			virtual bool Equals(const Type* other) const
			{
				return kind == other->kind;
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

		class PrimitiveType : public Type
		{
		private:
			std::string name;

		public:
			PrimitiveType(TypeKind k, const std::string& n) : Type(k), name(n) {}

			std::string ToString() const override
			{
				return name;
			}

			const std::string& GetName() const
			{
				return name;
			}
		};

		class ArrayType : public Type
		{
		private:
			std::shared_ptr<Type> elementType;
			size_t				  size;

		public:
			ArrayType(std::shared_ptr<Type> elemType, size_t sz = 0)
				: Type(TypeKind::Array), elementType(elemType), size(sz)
			{
			}

			std::string ToString() const override
			{
				if (size > 0)
				{
					return elementType->ToString() + "[" + std::to_string(size) + "]";
				}
				return elementType->ToString() + "[]";
			}

			std::shared_ptr<Type> GetElementType() const
			{
				return elementType;
			}

			size_t GetSize() const
			{
				return size;
			}

			bool Equals(const Type* other) const override
			{
				if (!Type::Equals(other))
				{
					return false;
				}

				auto arrayType = dynamic_cast<const ArrayType*>(other);
				return arrayType && elementType->Equals(arrayType->elementType.get()) &&
					   size == arrayType->size;
			}
		};

		class PointerType : public Type
		{
		private:
			std::shared_ptr<Type> pointeeType;

		public:
			PointerType(std::shared_ptr<Type> pointee) : Type(TypeKind::Pointer), pointeeType(pointee) {}

			std::string ToString() const override
			{
				return pointeeType->ToString() + "*";
			}

			std::shared_ptr<Type> GetPointeeType() const
			{
				return pointeeType;
			}

			bool Equals(const Type* other) const override
			{
				if (!Type::Equals(other))
				{
					return false;
				}

				auto pointerType = dynamic_cast<const PointerType*>(other);
				return pointerType && pointeeType->Equals(pointerType->pointeeType.get());
			}
		};

		class FunctionType : public Type
		{
		private:
			std::shared_ptr<Type>			   returnType;
			std::vector<std::shared_ptr<Type>> parameterTypes;

		public:
			FunctionType(std::shared_ptr<Type> retType, const std::vector<std::shared_ptr<Type>>& paramTypes)
				: Type(TypeKind::Function), returnType(retType), parameterTypes(paramTypes)
			{
			}

			std::string ToString() const override
			{
				std::string result = "(";
				for (size_t i = 0; i < parameterTypes.size(); ++i)
				{
					if (i > 0)
					{
						result += ", ";
					}
					result += parameterTypes[i]->ToString();
				}
				result += ") -> ";
				result += returnType->ToString();
				return result;
			}

			std::shared_ptr<Type> GetReturnType() const
			{
				return returnType;
			}

			const std::vector<std::shared_ptr<Type>>& GetParameterTypes() const
			{
				return parameterTypes;
			}

			bool Equals(const Type* other) const override
			{
				if (!Type::Equals(other))
				{
					return false;
				}

				auto funcType = dynamic_cast<const FunctionType*>(other);
				if (!funcType || !returnType->Equals(funcType->returnType.get()))
				{
					return false;
				}

				if (parameterTypes.size() != funcType->parameterTypes.size())
				{
					return false;
				}

				for (size_t i = 0; i < parameterTypes.size(); ++i)
				{
					if (!parameterTypes[i]->Equals(funcType->parameterTypes[i].get()))
					{
						return false;
					}
				}

				return true;
			}
		};

		class TypeRegistry
		{
		private:
			std::vector<std::shared_ptr<Type>> types;

		public:
			TypeRegistry() {}

			std::shared_ptr<Type> RegisterType(std::shared_ptr<Type> type)
			{
				types.push_back(type);
				return type;
			}

			std::shared_ptr<Type> FindType(const std::string& name) const
			{
				for (const auto& type : types)
				{
					if (type->ToString() == name)
					{
						return type;
					}
				}
				return nullptr;
			}

			void Clear()
			{
				types.clear();
			}
		};
	} // namespace semantic
} // namespace mdsl
