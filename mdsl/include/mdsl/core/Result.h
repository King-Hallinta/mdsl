#pragma once

#include <exception>
#include <functional>
#include <string>
#include <utility>
#include <variant>

namespace mdsl
{
	namespace core
	{
		enum class ErrorCode
		{
			Success = 0,
			LexerError,
			ParserError,
			SemanticError,
			CodeGenError,
			RuntimeError,
			IOError,
			UnknownError
		};

		struct Error
		{
			ErrorCode	code;
			std::string message;

			Error() : code(ErrorCode::Success) {}

			Error(ErrorCode c, const std::string& msg) : code(c), message(msg) {}

			explicit Error(const std::string& msg) : code(ErrorCode::UnknownError), message(msg) {}

			std::string ToString() const
			{
				return message;
			}
		};

		class ResultException : public std::exception
		{
		private:
			std::string msg;

		public:
			explicit ResultException(const std::string& message) : msg(message) {}

			const char* what() const noexcept override
			{
				return msg.c_str();
			}
		};

		template <typename T> class Result
		{
		private:
			std::variant<T, Error> m_data;

		public:
			Result(const T& value) : m_data(value) {}

			Result(T&& value) : m_data(std::move(value)) {}

			Result(const Error& error) : m_data(error) {}

			Result(Error&& error) : m_data(std::move(error)) {}

			bool IsOk() const
			{
				return std::holds_alternative<T>(m_data);
			}

			bool IsError() const
			{
				return std::holds_alternative<Error>(m_data);
			}

			const T& Value() const
			{
				return std::get<T>(m_data);
			}

			T& Value()
			{
				return std::get<T>(m_data);
			}

			const Error& GetError() const
			{
				return std::get<Error>(m_data);
			}

			Error& GetError()
			{
				return std::get<Error>(m_data);
			}

			T ValueOr(const T& default_value) const
			{
				return IsOk() ? Value() : default_value;
			}

			T&& Unwrap()
			{
				if (IsError())
				{
					throw ResultException("Called Unwrap() on Error: " + GetError().ToString());
				}
				return std::move(std::get<T>(m_data));
			}

			T UnwrapOr(const T& default_value)
			{
				return IsOk() ? Value() : default_value;
			}

			template <typename U> Result<U> Map(std::function<U(const T&)> mapper) const
			{
				if (IsOk())
				{
					return Result<U>(mapper(Value()));
				}
				return Result<U>(GetError());
			}

			template <typename U> Result<U> AndThen(std::function<Result<U>(const T&)> func) const
			{
				if (IsOk())
				{
					return func(Value());
				}
				return Result<U>(GetError());
			}

			Result<T> OrElse(std::function<Result<T>(const Error&)> func) const
			{
				if (IsError())
				{
					return func(GetError());
				}
				return *this;
			}
		};

		template <> class Result<void>
		{
		private:
			Error m_error_data;
			bool  m_has_error;

		public:
			Result() : m_has_error(false) {}

			Result(const Error& error) : m_error_data(error), m_has_error(true) {}

			Result(Error&& error) : m_error_data(std::move(error)), m_has_error(true) {}

			bool IsOk() const
			{
				return !m_has_error;
			}

			bool IsError() const
			{
				return m_has_error;
			}

			const Error& GetError() const
			{
				return m_error_data;
			}

			Error& GetError()
			{
				return m_error_data;
			}

			void Unwrap() const
			{
				if (IsError())
				{
					throw ResultException("Called Unwrap() on Error: " + m_error_data.ToString());
				}
			}
		};
	} // namespace core
} // namespace mdsl
