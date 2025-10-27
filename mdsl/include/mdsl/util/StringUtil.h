#pragma once

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

namespace mdsl
{
	namespace util
	{
		class StringUtil
		{
		public:
			static std::string Trim(const std::string& str)
			{
				size_t start = str.find_first_not_of(" \t\n\r");
				size_t end = str.find_last_not_of(" \t\n\r");

				if (start == std::string::npos || end == std::string::npos)
				{
					return "";
				}

				return str.substr(start, end - start + 1);
			}

			static std::string TrimLeft(const std::string& str)
			{
				size_t start = str.find_first_not_of(" \t\n\r");

				if (start == std::string::npos)
				{
					return "";
				}

				return str.substr(start);
			}

			static std::string TrimRight(const std::string& str)
			{
				size_t end = str.find_last_not_of(" \t\n\r");

				if (end == std::string::npos)
				{
					return "";
				}

				return str.substr(0, end + 1);
			}

			static std::vector<std::string> Split(const std::string& str, char delimiter)
			{
				std::vector<std::string> result;
				std::stringstream		 ss(str);
				std::string				 item;

				while (std::getline(ss, item, delimiter))
				{
					result.push_back(item);
				}

				return result;
			}

			static std::string Join(const std::vector<std::string>& strings, const std::string& delimiter)
			{
				if (strings.empty())
				{
					return "";
				}

				std::ostringstream result;
				result << strings[0];

				for (size_t i = 1; i < strings.size(); ++i)
				{
					result << delimiter << strings[i];
				}

				return result.str();
			}

			static std::string ToLower(const std::string& str)
			{
				std::string result = str;
				std::transform(result.begin(), result.end(), result.begin(), ::tolower);
				return result;
			}

			static std::string ToUpper(const std::string& str)
			{
				std::string result = str;
				std::transform(result.begin(), result.end(), result.begin(), ::toupper);
				return result;
			}

			static bool StartsWith(const std::string& str, const std::string& prefix)
			{
				if (prefix.length() > str.length())
				{
					return false;
				}

				return str.compare(0, prefix.length(), prefix) == 0;
			}

			static bool EndsWith(const std::string& str, const std::string& suffix)
			{
				if (suffix.length() > str.length())
				{
					return false;
				}

				return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
			}

			static std::string Replace(const std::string& str, const std::string& from, const std::string& to)
			{
				std::string result = str;
				size_t		pos = 0;

				while ((pos = result.find(from, pos)) != std::string::npos)
				{
					result.replace(pos, from.length(), to);
					pos += to.length();
				}

				return result;
			}

			static bool Contains(const std::string& str, const std::string& substring)
			{
				return str.find(substring) != std::string::npos;
			}

			template <typename T> static std::string ToString(const T& value)
			{
				std::ostringstream ss;
				ss << value;
				return ss.str();
			}
		};
	} // namespace util
} // namespace mdsl
