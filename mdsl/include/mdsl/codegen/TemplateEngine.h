#pragma once

#include <functional>
#include <sstream>
#include <string>
#include <unordered_map>

namespace mdsl
{
	namespace codegen
	{
		class TemplateContext
		{
		private:
			std::unordered_map<std::string, std::string>				  variables;
			std::unordered_map<std::string, std::function<std::string()>> functions;

		public:
			TemplateContext() {}

			void SetVariable(const std::string& name, const std::string& value)
			{
				variables[name] = value;
			}

			std::string GetVariable(const std::string& name) const
			{
				auto it = variables.find(name);
				if (it != variables.end())
				{
					return it->second;
				}
				return "";
			}

			void SetFunction(const std::string& name, std::function<std::string()> func)
			{
				functions[name] = func;
			}

			std::string CallFunction(const std::string& name) const
			{
				auto it = functions.find(name);
				if (it != functions.end())
				{
					return it->second();
				}
				return "";
			}

			bool HasVariable(const std::string& name) const
			{
				return variables.find(name) != variables.end();
			}

			bool HasFunction(const std::string& name) const
			{
				return functions.find(name) != functions.end();
			}
		};

		class TemplateEngine
		{
		private:
			std::string templateText;
			std::string openTag;
			std::string closeTag;

			std::string evaluateExpression(const std::string& expr, const TemplateContext& context) const
			{
				std::string trimmed = expr;

				size_t start = trimmed.find_first_not_of(" \t\n\r");
				size_t end = trimmed.find_last_not_of(" \t\n\r");

				if (start != std::string::npos && end != std::string::npos)
				{
					trimmed = trimmed.substr(start, end - start + 1);
				}

				if (trimmed.find("()") != std::string::npos)
				{
					std::string funcName = trimmed.substr(0, trimmed.find("()"));
					return context.CallFunction(funcName);
				}

				return context.GetVariable(trimmed);
			}

		public:
			TemplateEngine() : openTag("{{"), closeTag("}}") {}

			TemplateEngine(const std::string& open, const std::string& close) : openTag(open), closeTag(close)
			{
			}

			void SetTemplate(const std::string& text)
			{
				templateText = text;
			}

			void SetTags(const std::string& open, const std::string& close)
			{
				openTag = open;
				closeTag = close;
			}

			std::string Render(const TemplateContext& context) const
			{
				std::ostringstream result;
				size_t			   pos = 0;

				while (pos < templateText.length())
				{
					size_t openPos = templateText.find(openTag, pos);

					if (openPos == std::string::npos)
					{
						result << templateText.substr(pos);
						break;
					}

					result << templateText.substr(pos, openPos - pos);

					size_t closePos = templateText.find(closeTag, openPos + openTag.length());

					if (closePos == std::string::npos)
					{
						result << templateText.substr(openPos);
						break;
					}

					std::string expression = templateText.substr(
						openPos + openTag.length(), closePos - openPos - openTag.length());

					result << evaluateExpression(expression, context);

					pos = closePos + closeTag.length();
				}

				return result.str();
			}

			void Clear()
			{
				templateText.clear();
			}
		};
	} // namespace codegen
} // namespace mdsl
