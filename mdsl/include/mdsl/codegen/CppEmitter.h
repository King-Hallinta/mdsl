#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace mdsl
{
	namespace codegen
	{
		class CppEmitter
		{
		private:
			std::ostringstream output;
			int				   indentLevel;
			bool			   needsIndent;

			void WriteIndent()
			{
				if (needsIndent)
				{
					for (int i = 0; i < indentLevel; ++i)
					{
						output << "    ";
					}
					needsIndent = false;
				}
			}

		public:
			CppEmitter() : indentLevel(0), needsIndent(true) {}

			void WriteLine(const std::string& line = "")
			{
				WriteIndent();
				output << line << "\n";
				needsIndent = true;
			}

			void Write(const std::string& text)
			{
				WriteIndent();
				output << text;
			}

			void BeginBlock()
			{
				WriteLine("{");
				indentLevel++;
			}

			void EndBlock(bool semicolon = false)
			{
				indentLevel--;
				if (semicolon)
				{
					WriteLine("};");
				}
				else
				{
					WriteLine("}");
				}
			}

			void WriteInclude(const std::string& header, bool systemHeader = true)
			{
				if (systemHeader)
				{
					WriteLine("#include <" + header + ">");
				}
				else
				{
					WriteLine("#include \"" + header + "\"");
				}
			}

			void WriteNamespaceBegin(const std::string& name)
			{
				WriteLine("namespace " + name);
				BeginBlock();
			}

			void WriteNamespaceEnd()
			{
				EndBlock();
			}

			void WriteClass(const std::string& name, const std::vector<std::string>& baseClasses = {})
			{
				std::string line = "class " + name;

				if (!baseClasses.empty())
				{
					line += " : ";
					for (size_t i = 0; i < baseClasses.size(); ++i)
					{
						if (i > 0)
						{
							line += ", ";
						}
						line += "public " + baseClasses[i];
					}
				}

				WriteLine(line);
				BeginBlock();
			}

			void WriteStruct(const std::string& name)
			{
				WriteLine("struct " + name);
				BeginBlock();
			}

			void WritePublic()
			{
				indentLevel--;
				WriteLine("public:");
				indentLevel++;
			}

			void WritePrivate()
			{
				indentLevel--;
				WriteLine("private:");
				indentLevel++;
			}

			void WriteProtected()
			{
				indentLevel--;
				WriteLine("protected:");
				indentLevel++;
			}

			void WriteFunction(const std::string& returnType,
				const std::string&				  name,
				const std::vector<std::string>&	  parameters,
				const std::vector<std::string>&	  modifiers = {})
			{
				std::string line = returnType + " " + name + "(";

				for (size_t i = 0; i < parameters.size(); ++i)
				{
					if (i > 0)
					{
						line += ", ";
					}
					line += parameters[i];
				}

				line += ")";

				if (!modifiers.empty())
				{
					for (const auto& mod : modifiers)
					{
						line += " " + mod;
					}
				}

				WriteLine(line);
				BeginBlock();
			}

			void EndFunction()
			{
				EndBlock();
			}

			void WriteStatement(const std::string& statement)
			{
				WriteLine(statement + ";");
			}

			void WriteReturn(const std::string& value = "")
			{
				if (value.empty())
				{
					WriteStatement("return");
				}
				else
				{
					WriteStatement("return " + value);
				}
			}

			void WriteVariable(
				const std::string& type, const std::string& name, const std::string& initializer = "")
			{
				if (initializer.empty())
				{
					WriteStatement(type + " " + name);
				}
				else
				{
					WriteStatement(type + " " + name + " = " + initializer);
				}
			}

			void WriteComment(const std::string& comment)
			{
				WriteLine("// " + comment);
			}

			std::string ToString() const
			{
				return output.str();
			}

			void Clear()
			{
				output.str("");
				output.clear();
				indentLevel = 0;
				needsIndent = true;
			}
		};

	} // namespace codegen
} // namespace mdsl
