#ifndef ARRAY_H
#define ARRAY_H

#include <stdio.h>
#include <cstdlib>

template<class T>
class Array {

public:

	Array();
	Array(const Array &a);
	~Array();

	Array& operator = (const Array &a);

	T& operator [] (int index);

	void add(const T &item);
	unsigned int getSize();
	void setSize(unsigned int newsize);
	void clear();
	void removeAt(unsigned int pos);
	int find(const T &item);

private:
	const static int SIZE_BASE = 128;
	const static int SIZE_EXPAND_MULTIPLIER = 2;

	T *array;
	unsigned int size;
	unsigned int allocatedSize;

};

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

template <class T>
Array<T>::Array(){
	allocatedSize = SIZE_BASE;
							  
	size = 0;
	array = (T *)malloc(allocatedSize*sizeof(T));
}

template <class T>
Array<T>::~Array(){
	if (array){
		free(array);
		array = NULL;
	}
}

template <class T>
Array<T>::Array(const Array &a){
	array = (T *)malloc(sizeof(T)*a.allocatedSize);
	memcpy(array, a.array, sizeof(T)*a.allocatedSize);
	allocatedSize = a.allocatedSize;
	size = a.size;
}

template <class T>
Array<T>& Array<T>::operator = (const Array &a){
	if (&a == this) {
		return *this;
	}

	if (a.size == 0) {
		clear();
	}

	setSize(a.size);
	memcpy(array, a.array, sizeof(T) * a.size);

	return *this;
}

template <class T>
T& Array<T>::operator [] (int index){
	return array[index];
}

template <class T>
void Array<T>::add(const T &item){
	size++;

	if (size > allocatedSize){
		allocatedSize *= SIZE_EXPAND_MULTIPLIER;
		
		array = (T *)realloc(array, sizeof(T)*allocatedSize);
	}

	array[size-1] = item;
}

template <class T>
unsigned int Array<T>::getSize(){
	return size;
}

template <class T>
void Array<T>::setSize(unsigned int newSize){
	size = newSize;

	if (size != 0){
		if ((size > allocatedSize) || (size < allocatedSize/2)){
			allocatedSize = size;
			array = (T *)realloc(array, sizeof(T)*size);
		}
	}
	else {
		clear();
	}
}

template <class T>
void Array<T>::clear() {
	size = 0; 
	array = (T *)realloc(array, sizeof(T)*SIZE_BASE); 
	allocatedSize = SIZE_BASE;
}

template <class T>
void Array<T>::removeAt(unsigned int pos){
	if (size == 1) {
		clear();
	}
	else{
		for(unsigned int i = pos; i < size - 1; i++) {
			array[i] = array[i+1];
		}
		size--;
	}
}

template <class T>
int Array<T>::find(const T &item){
	for (int i = 0; i < size; ++i){
		if (array[i] == item ) {
			return i;
		}
	}
	return -1;
}


#endif //ARRAY_H