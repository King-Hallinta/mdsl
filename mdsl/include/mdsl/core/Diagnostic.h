#pragma once

#include "SourceLocation.h"

#include <memory>
#include <string>
#include <vector>

namespace mdsl
{
	namespace core
	{
		enum class DiagnosticSeverity
		{
			Note,
			Warning,
			Error,
			Fatal
		};

		struct Diagnostic
		{
			DiagnosticSeverity		 severity;
			std::string				 message;
			SourceSpan				 location;
			std::vector<std::string> notes;

			Diagnostic(DiagnosticSeverity sev, const std::string& msg, const SourceSpan& loc = SourceSpan())
				: severity(sev), message(msg), location(loc)
			{
			}

			void AddNote(const std::string& note)
			{
				notes.push_back(note);
			}

			std::string ToString() const;
		};

		class IDiagnosticReporter
		{
		public:
			virtual ~IDiagnosticReporter() = default;

			virtual void   Report(const Diagnostic& diagnostic) = 0;
			virtual void   Flush() = 0;
			virtual bool   HasErrors() const = 0;
			virtual size_t GetErrorCount() const = 0;
			virtual size_t GetWarningCount() const = 0;
		};

		class DiagnosticEngine
		{
		private:
			std::shared_ptr<IDiagnosticReporter> reporter;
			bool								 treatWarningsAsErrors;

		public:
			DiagnosticEngine(std::shared_ptr<IDiagnosticReporter> rep = nullptr)
				: reporter(rep), treatWarningsAsErrors(false)
			{
			}

			void SetReporter(std::shared_ptr<IDiagnosticReporter> rep)
			{
				reporter = rep;
			}

			void SetTreatWarningsAsErrors(bool value)
			{
				treatWarningsAsErrors = value;
			}

			void ReportNote(const std::string& message, const SourceSpan& location = SourceSpan())
			{
				if (reporter)
				{
					reporter->Report(Diagnostic(DiagnosticSeverity::Note, message, location));
				}
			}

			void ReportWarning(const std::string& message, const SourceSpan& location = SourceSpan())
			{
				if (reporter)
				{
					DiagnosticSeverity severity =
						treatWarningsAsErrors ? DiagnosticSeverity::Error : DiagnosticSeverity::Warning;
					reporter->Report(Diagnostic(severity, message, location));
				}
			}

			void ReportError(const std::string& message, const SourceSpan& location = SourceSpan())
			{
				if (reporter)
				{
					reporter->Report(Diagnostic(DiagnosticSeverity::Error, message, location));
				}
			}

			void ReportFatal(const std::string& message, const SourceSpan& location = SourceSpan())
			{
				if (reporter)
				{
					reporter->Report(Diagnostic(DiagnosticSeverity::Fatal, message, location));
				}
			}

			bool HasErrors() const
			{
				return reporter ? reporter->HasErrors() : false;
			}

			size_t GetErrorCount() const
			{
				return reporter ? reporter->GetErrorCount() : 0;
			}

			size_t GetWarningCount() const
			{
				return reporter ? reporter->GetWarningCount() : 0;
			}

			void Flush()
			{
				if (reporter)
				{
					reporter->Flush();
				}
			}
		};

		class DefaultDiagnosticReporter : public IDiagnosticReporter
		{
		private:
			std::vector<Diagnostic> diagnostics;
			size_t					errorCount;
			size_t					warningCount;

		public:
			DefaultDiagnosticReporter() : errorCount(0), warningCount(0) {}

			void Report(const Diagnostic& diagnostic) override
			{
				diagnostics.push_back(diagnostic);

				if (diagnostic.severity == DiagnosticSeverity::Error ||
					diagnostic.severity == DiagnosticSeverity::Fatal)
				{
					errorCount++;
				}
				else if (diagnostic.severity == DiagnosticSeverity::Warning)
				{
					warningCount++;
				}
			}

			void Flush() override {}

			bool HasErrors() const override
			{
				return errorCount > 0;
			}

			size_t GetErrorCount() const override
			{
				return errorCount;
			}

			size_t GetWarningCount() const override
			{
				return warningCount;
			}

			const std::vector<Diagnostic>& GetDiagnostics() const
			{
				return diagnostics;
			}

			void Clear()
			{
				diagnostics.clear();
				errorCount = 0;
				warningCount = 0;
			}
		};
	} // namespace core
} // namespace mdsl
