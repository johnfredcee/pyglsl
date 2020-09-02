
#ifndef MEMORY_CATEGORY_H_
#define MEMORY_CATEGORY_H_

#include <Memory/MemoryCategoryCore.h>

enum MemoryCategory {
	// MEM_PERIPHERALS From MemoryCategoryCore				// 0
	// VERTEX_BUFFERS From MemoryCategoryCore				// 1
	// VERTEX_INDEX_BUFFERS From MemoryCategoryCore			// 2
	// MEM_MATHS From MemoryCategoryCore					// 3
	// MEM_DEBUGGING From MemoryCategoryCore				// 4
	// MEM_LUA From MemoryCategoryCore						// 5
	// MEM_AUDIO_SAMPLES MemoryCategoryCore					// 6
	// MEM_AUDIO_BUFFERS MemoryCategoryCore					// 7
	// MEM_FILE_IO From MemoryCategoryCore					// 8
	// MEM_MESSAGES From MemoryCategoryCore					// 9
	// MEM_COMPONENTS From MemoryCategoryCore				// 10
	// MEM_DATABASE From MemoryCategoryCore					// 11
	// MEM_NETWORK,											// 12
	// MEM_STRINGS,											// 13
	// MEM_CORE_DEFAULTS from MemoryCategoryCore			// 14
	
	MEM_END = MEM_CORE_DEFAULTS
};

#endif
