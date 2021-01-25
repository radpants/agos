//
// Created by AJ Austinson on 1/5/2021.
//

#include "array.h"

#define ARRAY_DEFAULT_CAPACITY 32

Array ArrayMake(size_t elementSize) {
	Array a = (Array){
		.data = malloc(elementSize * ARRAY_DEFAULT_CAPACITY),
		.elementSize = elementSize,
		.count = 0  ,
		.capacity = ARRAY_DEFAULT_CAPACITY
	};
	return a;
}

Array *ArrayNew(size_t elementSize) {
	Array* array = (Array*)malloc(sizeof(Array));
	array->data = malloc(elementSize * ARRAY_DEFAULT_CAPACITY);
	array->elementSize = elementSize;
	array->count = 0;
	array->capacity = ARRAY_DEFAULT_CAPACITY;
	return array;
}

void ArrayFree(Array *array) {
	free(array->data);
	free(array);
}

void ArrayResize(Array *array, size_t newSize) {
	assert(newSize >= array->count);
	array->data = realloc(array->data, array->elementSize * newSize);
	assert(array->data);
	array->capacity = newSize;
}

void ArrayPush(Array *array, void *element) {
	if(array->count == array->capacity) {
		ArrayResize(array, array->capacity * 2); // TODO: This is a really silly way to guess at a new size
	}
	memcpy((uintptr_t)array->data + array->count * array->elementSize, element, array->elementSize);
	array->count++;
}
