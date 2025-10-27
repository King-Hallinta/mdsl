#pragma once

#include <string>
#include <string_view>
#include <unordered_set>

namespace mdsl
{
	namespace core
	{
		class StringPool
		{
		private:
			std::unordered_set<std::string> pool;

		public:
			StringPool() {}

			const char* Intern(const std::string& str)
			{
				auto it = pool.find(str);
				if (it != pool.end())
				{
					return it->c_str();
				}

				auto result = pool.insert(str);
				return result.first->c_str();
			}

			const char* Intern(std::string_view str)
			{
				auto it = pool.find(std::string(str));
				if (it != pool.end())
				{
					return it->c_str();
				}

				auto result = pool.insert(std::string(str));
				return result.first->c_str();
			}

			const char* Intern(const char* str)
			{
				return Intern(std::string(str));
			}

			void Clear()
			{
				pool.clear();
			}

			size_t Size() const
			{
				return pool.size();
			}
		};
	} // namespace core
} // namespace mdsl
