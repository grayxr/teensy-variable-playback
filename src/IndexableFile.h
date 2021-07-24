#ifndef TEENSY_RESAMPLING_INDEXABLE_FILE_H
#define TEENSY_RESAMPLING_INDEXABLE_FILE_H

#include <Arduino.h>
#include <SD.h>
#include <vector>

namespace newdigate {


struct indexedbuffer {
    uint32_t index;
    uint16_t buffer_size;
    int16_t *buffer;
};

constexpr bool isPowerOf2(size_t value){
    return !(value == 0) && !(value & (value - 1));
}

template<size_t BUFFER_SIZE, size_t MAX_NUM_BUFFERS> // BUFFER_SIZE needs to be a power of two
class IndexableFile {
public:
    static_assert(isPowerOf2(BUFFER_SIZE), "BUFFER_SIZE must be a power of 2");

    static constexpr size_t element_size = sizeof(int16_t);
    size_t buffer_to_index_shift;
    IndexableFile(File& file) : 
        _file(file), 
        _buffers(),
        buffer_to_index_shift(log2(BUFFER_SIZE)) {
    }

    int16_t &operator[](int i) {
        uint32_t indexFor_i = i >> buffer_to_index_shift;
        indexedbuffer *match = find_with_index(indexFor_i);
        if (match == nullptr) {
            indexedbuffer *next = new indexedbuffer();
            next->index = indexFor_i;
            next->buffer = new int16_t[BUFFER_SIZE];
            size_t basePos = indexFor_i << buffer_to_index_shift;
            size_t seekPos = basePos * element_size;
            _file.seek(seekPos);
            unsigned bytesRead = _file.read(next->buffer, BUFFER_SIZE * element_size);
            next->buffer_size = bytesRead;
            _buffers.push_back(next);
            match = next;

            if (_buffers.size() > MAX_NUM_BUFFERS) {
                indexedbuffer *first = *_buffers.begin();
                _buffers.erase(_buffers.begin());
                delete [] first->buffer;
                delete first;
            }
        }
        return match->buffer[i % BUFFER_SIZE];
    }

private:
    File _file;
    std::vector<indexedbuffer*> _buffers;

    indexedbuffer* find_with_index(uint32_t i) {
        for (auto && x : _buffers){
            if (x->index == i) {
                return x;
            }
        }
        return nullptr;
    }
};

}

#endif 
