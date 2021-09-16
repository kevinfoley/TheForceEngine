#pragma once
//////////////////////////////////////////////////////////////////////
// General purpose memory allocator which acts as a region of
// memory which can be quickly cleared.
//////////////////////////////////////////////////////////////////////
#include <TFE_System/types.h>
#include <TFE_FileSystem/filestream.h>
#include <vector>
#include <string>

struct MemoryRegion;
typedef u32 RelativePointer;

#define NULL_RELATIVE_POINTER 0

namespace TFE_Memory
{
	MemoryRegion* region_create(const char* name, size_t blockSize, size_t maxSize = 0u);
	void region_clear(MemoryRegion* region);
	void region_destroy(MemoryRegion* region);

	void* region_alloc(MemoryRegion* region, size_t size);
	void* region_realloc(MemoryRegion* region, void* ptr, size_t size);
	void  region_free(MemoryRegion* region, void* ptr);

	RelativePointer region_getRelativePointer(MemoryRegion* region, void* ptr);
	void* region_getRealPointer(MemoryRegion* region, RelativePointer ptr);

	// TODO: Support writing to and restoring from streams in memory.
	bool region_serializeToDisk(MemoryRegion* region, FileStream* file);
	MemoryRegion* region_restoreFromDisk(FileStream* file);

	void region_test();
}