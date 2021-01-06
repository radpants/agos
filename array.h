//
// Created by AJ Austinson on 1/5/2021.
//

#ifndef AGOS_ARRAY_H
#define AGOS_ARRAY_H

typedef struct {
	void* data;
	size_t capacity, count, elementSize;
} Array;

Array* ArrayNew(size_t elementSize);
void ArrayFree(Array* array);

void ArrayResize(Array* array, size_t newSize);
void ArrayPush(Array* array, void* element);

static inline uintptr_t ArrayGet(Array* array, size_t index) {
	assert(index < array->count);
	return (uintptr_t)((uintptr_t)array->data + array->elementSize * index);
}

#endif //AGOS_ARRAY_H
