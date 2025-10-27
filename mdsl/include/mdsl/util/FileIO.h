#pragma once

#include "../core/Result.h"

#include <fstream>
#include <string>
#include <vector>

namespace mdsl
{
	namespace util
	{
		class FileIO
		{
		public:
			static core::Result<std::string> ReadFile(const std::string& filename)
			{
				std::ifstream file(filename, std::ios::binary);

				if (!file.is_open())
				{
					return core::Result<std::string>(
						core::Error(core::ErrorCode::IOError, "Failed to open file: " + filename));
				}

				file.seekg(0, std::ios::end);
				size_t size = file.tellg();
				file.seekg(0, std::ios::beg);

				std::string content(size, '\0');
				file.read(&content[0], size);

				if (file.fail())
				{
					return core::Result<std::string>(
						core::Error(core::ErrorCode::IOError, "Failed to read file: " + filename));
				}

				return core::Result<std::string>(content);
			}

			static core::Result<void> WriteFile(const std::string& filename, const std::string& content)
			{
				std::ofstream file(filename, std::ios::binary);

				if (!file.is_open())
				{
					return core::Result<void>(core::Error(
						core::ErrorCode::IOError, "Failed to open file for writing: " + filename));
				}

				file.write(content.c_str(), content.length());

				if (file.fail())
				{
					return core::Result<void>(
						core::Error(core::ErrorCode::IOError, "Failed to write file: " + filename));
				}

				return core::Result<void>();
			}

			static core::Result<void> WriteBinaryFile(
				const std::string& filename, const std::vector<uint8_t>& data)
			{
				std::ofstream file(filename, std::ios::binary);

				if (!file.is_open())
				{
					return core::Result<void>(core::Error(
						core::ErrorCode::IOError, "Failed to open file for writing: " + filename));
				}

				file.write(reinterpret_cast<const char*>(data.data()), data.size());

				if (file.fail())
				{
					return core::Result<void>(
						core::Error(core::ErrorCode::IOError, "Failed to write binary file: " + filename));
				}

				return core::Result<void>();
			}

			static core::Result<std::vector<uint8_t>> ReadBinaryFile(const std::string& filename)
			{
				std::ifstream file(filename, std::ios::binary);

				if (!file.is_open())
				{
					return core::Result<std::vector<uint8_t>>(
						core::Error(core::ErrorCode::IOError, "Failed to open file: " + filename));
				}

				file.seekg(0, std::ios::end);
				size_t size = file.tellg();
				file.seekg(0, std::ios::beg);

				std::vector<uint8_t> data(size);
				file.read(reinterpret_cast<char*>(data.data()), size);

				if (file.fail())
				{
					return core::Result<std::vector<uint8_t>>(
						core::Error(core::ErrorCode::IOError, "Failed to read binary file: " + filename));
				}

				return core::Result<std::vector<uint8_t>>(data);
			}

			static bool FileExists(const std::string& filename)
			{
				std::ifstream file(filename);
				return file.good();
			}
		};
	} // namespace util
} // namespace mdsl
