#include "mdsl/core/Diagnostic.h"

namespace mdsl
{
	namespace core
	{
		std::string SeverityToString(DiagnosticSeverity severity)
		{
			switch (severity)
			{
				case DiagnosticSeverity::Note:
					return "note";
				case DiagnosticSeverity::Warning:
					return "warning";
				case DiagnosticSeverity::Error:
					return "error";
				case DiagnosticSeverity::Fatal:
					return "fatal";
				default:
					return "unknown";
			}
		}

		std::string Diagnostic::ToString() const
		{
			std::string result;

			if (location.IsValid())
			{
				result += location.ToString() + ": ";
			}

			result += SeverityToString(severity) + ": " + message;

			if (!notes.empty())
			{
				result += "\n";
				for (const auto& note : notes)
				{
					result += "  note: " + note + "\n";
				}
			}

			return result;
		}

	} // namespace core
} // namespace mdsl
