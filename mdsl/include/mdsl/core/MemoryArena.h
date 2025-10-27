#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace mdsl
{
	namespace core
	{
		class MemoryArena
		{
		private:
			struct Block
			{
				uint8_t* data;
				size_t	 size;
				size_t	 used;

				Block(size_t blockSize) : size(blockSize), used(0)
				{
					data = new uint8_t[blockSize];
				}

				~Block()
				{
					delete[] data;
				}

				Block(const Block&) = delete;
				Block& operator=(const Block&) = delete;
			};

			std::vector<std::unique_ptr<Block>> blocks;
			size_t								blockSize;
			size_t								currentBlockIndex;

			Block* GetCurrentBlock()
			{
				if (blocks.empty())
				{
					blocks.push_back(std::make_unique<Block>(blockSize));
					currentBlockIndex = 0;
				}
				return blocks[currentBlockIndex].get();
			}

			void* allocateFromBlock(size_t size, size_t alignment)
			{
				Block* block = GetCurrentBlock();

				uintptr_t current = reinterpret_cast<uintptr_t>(block->data + block->used);
				uintptr_t aligned = (current + alignment - 1) & ~(alignment - 1);
				size_t	  padding = aligned - current;
				size_t	  totalSize = padding + size;

				if (block->used + totalSize > block->size)
				{
					blocks.push_back(std::make_unique<Block>(blockSize > size ? blockSize : size * 2));
					currentBlockIndex = blocks.size() - 1;
					block = GetCurrentBlock();

					current = reinterpret_cast<uintptr_t>(block->data);
					aligned = (current + alignment - 1) & ~(alignment - 1);
					padding = aligned - current;
					totalSize = padding + size;
				}

				void* result = reinterpret_cast<void*>(aligned);
				block->used += totalSize;
				return result;
			}

		public:
			explicit MemoryArena(size_t defaultBlockSize = 65536)
				: blockSize(defaultBlockSize), currentBlockIndex(0)
			{
			}

			~MemoryArena() {}

			MemoryArena(const MemoryArena&) = delete;
			MemoryArena& operator=(const MemoryArena&) = delete;

			void* Allocate(size_t size, size_t alignment = alignof(std::max_align_t))
			{
				return allocateFromBlock(size, alignment);
			}

			template <typename T, typename... Args> T* create(Args&&... args)
			{
				void* memory = Allocate(sizeof(T), alignof(T));
				return new (memory) T(std::forward<Args>(args)...);
			}

			void Reset()
			{
				for (auto& block : blocks)
				{
					block->used = 0;
				}
				currentBlockIndex = 0;
			}

			size_t GetTotalAllocated() const
			{
				size_t total = 0;
				for (const auto& block : blocks)
				{
					total += block->used;
				}
				return total;
			}

			size_t GetTotalCapacity() const
			{
				size_t total = 0;
				for (const auto& block : blocks)
				{
					total += block->size;
				}
				return total;
			}
		};
	} // namespace core
} // namespace mdsl
