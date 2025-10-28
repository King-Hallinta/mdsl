#pragma once

#include <cstdint>
#include <cstring>
#include <sstream>
#include <string>

namespace mdsl
{
	namespace core
	{
		struct SourceLocation
		{
			const char* filename;
			uint32_t	line;
			uint32_t	column;

			SourceLocation() : filename(nullptr), line(0), column(0) {}

			SourceLocation(const char* file, uint32_t ln, uint32_t col)
				: filename(file), line(ln), column(col)
			{
			}

			bool IsValid() const
			{
				return filename != nullptr && line > 0;
			}

			std::string ToString() const
			{
				if (!IsValid())
				{
					return "<invalid location>";
				}

				std::ostringstream oss;
				oss << filename << ":" << line << ":" << column;
				return oss.str();
			}
		};

		struct SourceSpan
		{
			SourceLocation start;
			SourceLocation end;

			SourceSpan() {}

			SourceSpan(const SourceLocation& s, const SourceLocation& e) : start(s), end(e) {}

			SourceSpan(const SourceLocation& loc) : start(loc), end(loc) {}

			bool IsValid() const
			{
				return start.IsValid();
			}

			std::string ToString() const
			{
				if (!IsValid())
				{
					return "<invalid span>";
				}

				if (start.filename && end.filename && std::strcmp(start.filename, end.filename) == 0 &&
					start.line == end.line)
				{
					std::ostringstream oss;
					oss << start.filename << ":" << start.line << ":" << start.column << "-" << end.column;
					return oss.str();
				}

				return start.ToString() + " to " + end.ToString();
			}
		};

	} // namespace core
} // namespace mdsl
