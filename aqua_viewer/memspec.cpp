#include <cstddef>
#include <Utils/PbAssert.h>
#include <PbSystemTypes.h>
#include <Memory/MemoryManager.h>
#include "MemoryCategory.h"

using namespace aqua;

bool LOG_ALLOCATIONS = false;
bool LOG_DEALLOCATIONS = false;

using std::size_t;

size_t ALLOCATORS[] = {
    ALLOCATOR_PLATFORM_MALLOC,       0,     0,     // 0 = MEM_DEFAULT
};

// these *must* be in the same order as they are defined in MemoryCategoryCore and extended in MemoryCategory
// (but they don't include MEM_DEFAULT which tries to find the smallest fitting block)
size_t CATEGORY_LOCATIONS[] = {
		MEM_DEFAULT,                    0,  0,
		MEM_SYSALLOC,                   0,  0,                                                                                  // 1
		MEM_PERIPHERALS,                0,  0,																					// 2
		MEM_VERTEX_BUFFERS,             0,  0,																					// 3
		MEM_VERTEX_INDEX_BUFFERS,       0,  0,																					// 4
		MEM_MATHS_VECTOR,               0,  0,																					// 5
		MEM_MATHS_MATRIX,               0,  0,			// Anything that needs to be stored for debugging only!.				// 6
		MEM_LUA,                        0,  0,																					// 7
		MEM_AUDIO_BUFFERS,              0,  0,				// Variable data													// 8
		MEM_AUDIO_SAMPLES,              0,  0,				// Fixed data														// 9
		MEM_FILE_IO,                    0,  0,																					// 10
		MEM_MESSAGES,                   0,  0,																			        // 11
		MEM_COMPONENTS,                 0,  0,																					// 12
		MEM_DATABASE,                   0,  0,																					// 13
		MEM_NETWORK,                    0,  0,																					// 14
		MEM_STRINGS,                    0,  0,																					// 14
};



// *** DO NOT MODIFY ANYTHING BELOW THIS LINE ***

size_t NUM_ALLOCATORS(sizeof(ALLOCATORS) / sizeof(size_t) / 3);
size_t NUM_CATEGORIES(MEM_END);
size_t NUM_CATEGORY_LOCATIONS(sizeof(CATEGORY_LOCATIONS) / sizeof(size_t) / 3);

void categoryCheck() {
	PBSTATIC_ASSERT(( sizeof(CATEGORY_LOCATIONS) / sizeof(size_t) / 3) < MEM_END + 1,      There_are_more_CATEGORY_LOCATIONS_than_there_are_memory_categories);
	PBSTATIC_ASSERT(((sizeof(CATEGORY_LOCATIONS) / sizeof(size_t) / 3)) > MEM_END - 1, There_are_less_CATEGORY_LOCATIONS_than_there_are_memory_categories);
}


