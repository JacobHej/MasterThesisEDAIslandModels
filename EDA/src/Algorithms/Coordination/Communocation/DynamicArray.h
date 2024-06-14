#pragma once

#include <memory>
#include <algorithm>

template<class T>
class DynamicArray {
public:
    DynamicArray(size_t size) {
        _data = std::make_unique<T[]>(size);
        _data_size = size;
    }

    DynamicArray(size_t size, T init_val) : DynamicArray(size) {
        for (int i = 0; i < size; i++) _data[i] = init_val;
    }

    DynamicArray(T* data, size_t size) : DynamicArray(size) {
        std::copy(data, data + size, _data.get());
    }

    DynamicArray(const std::unique_ptr<T[]>& data, size_t size) : DynamicArray(data.get(), size) { }

    DynamicArray(std::unique_ptr<T[]>&& data, size_t size) : DynamicArray(size) {
        _data = std::move(data);
    }

    DynamicArray(const DynamicArray<T>& arr) : DynamicArray(arr._data, _data_size) { }

    DynamicArray(DynamicArray<T>&& arr) : DynamicArray(std::move(arr._data), arr._data_size) {
        arr._data_size = -1;
    }

    inline T operator [] (size_t index) const {
        return _data[index];
    }

    inline T& operator [] (size_t index) {
        return _data[index];
    }

    inline size_t size() {
        return _data_size;
    }

    void resize(size_t new_size) {
        std::unique_ptr<T[]> new_data = std::make_unique<T[]>(new_size);
        std::copy(_data.get(), _data.get() + (_data_size > new_size ? new_size : _data_size), new_data.get());
        _data = std::move(new_data);
        _data_size = new_size;
    }

    void concat(const DynamicArray<T>& other) {
        resize(_data_size + other._data_size);
        std::copy(other._data.get(), other._data.get() + other._data_size, _data.get() + _data_size - other._data_size);
    }

    inline T* get() {        
        return _data.get();
    }

private:
    std::unique_ptr<T[]> _data;
    size_t _data_size;
};