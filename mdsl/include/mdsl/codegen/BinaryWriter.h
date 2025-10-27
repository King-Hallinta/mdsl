#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace mdsl
{
	namespace codegen
	{
		class BinaryWriter
		{
		private:
			std::vector<uint8_t> buffer;

		public:
			BinaryWriter() {}

			void Write(uint8_t value)
			{
				buffer.push_back(value);
			}

			void Write(uint16_t value)
			{
				Write(static_cast<uint8_t>(value & 0xFF));
				Write(static_cast<uint8_t>((value >> 8) & 0xFF));
			}

			void Write(uint32_t value)
			{
				Write(static_cast<uint16_t>(value & 0xFFFF));
				Write(static_cast<uint16_t>((value >> 16) & 0xFFFF));
			}

			void Write(uint64_t value)
			{
				Write(static_cast<uint32_t>(value & 0xFFFFFFFF));
				Write(static_cast<uint32_t>((value >> 32) & 0xFFFFFFFF));
			}

			void Write(int8_t value)
			{
				Write(static_cast<uint8_t>(value));
			}

			void Write(int16_t value)
			{
				Write(static_cast<uint16_t>(value));
			}

			void Write(int32_t value)
			{
				Write(static_cast<uint32_t>(value));
			}

			void Write(int64_t value)
			{
				Write(static_cast<uint64_t>(value));
			}

			void Write(float value)
			{
				uint32_t bits;
				std::memcpy(&bits, &value, sizeof(float));
				Write(bits);
			}

			void Write(double value)
			{
				uint64_t bits;
				std::memcpy(&bits, &value, sizeof(double));
				Write(bits);
			}

			void Write(const std::string& str)
			{
				Write(static_cast<uint32_t>(str.length()));
				for (char c : str)
				{
					Write(static_cast<uint8_t>(c));
				}
			}

			void Write(const void* data, size_t size)
			{
				const uint8_t* bytes = static_cast<const uint8_t*>(data);
				for (size_t i = 0; i < size; ++i)
				{
					Write(bytes[i]);
				}
			}

			void WriteAlign(size_t alignment)
			{
				while (buffer.size() % alignment != 0)
				{
					Write(static_cast<uint8_t>(0));
				}
			}

			size_t GetPosition() const
			{
				return buffer.size();
			}

			void Seek(size_t position)
			{
				if (position > buffer.size())
				{
					buffer.resize(position);
				}
			}

			void Patch(size_t position, uint32_t value)
			{
				if (position + 4 <= buffer.size())
				{
					buffer[position] = static_cast<uint8_t>(value & 0xFF);
					buffer[position + 1] = static_cast<uint8_t>((value >> 8) & 0xFF);
					buffer[position + 2] = static_cast<uint8_t>((value >> 16) & 0xFF);
					buffer[position + 3] = static_cast<uint8_t>((value >> 24) & 0xFF);
				}
			}

			const std::vector<uint8_t>& GetBuffer() const
			{
				return buffer;
			}

			void Clear()
			{
				buffer.clear();
			}

			size_t Size() const
			{
				return buffer.size();
			}
		};
	} // namespace codegen
} // namespace mdsl
