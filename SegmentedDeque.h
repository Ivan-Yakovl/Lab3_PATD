#ifndef SEGMENTED_DEQUE_H
#define SEGMENTED_DEQUE_H

#include "ListSequence.h"
#include "ArraySequence.h"
#include <fstream>
#include <stdexcept>

template <class T>
class SegmentedDeque {
private:
    ListSequence<ArraySequence<T>*>* segments;  
    int segmentSize;                          
    int frontIndex;                           
    int backIndex;                          
    int size;                                 

    void expandFront() {
        ArraySequence<T>* newSegment = new MutableArraySequence<T>(segmentSize);
        segments->Prepend(newSegment);
        frontIndex = segmentSize - 1;
    }

    void expandBack() {
        ArraySequence<T>* newSegment = new MutableArraySequence<T>(segmentSize);
        segments->Append(newSegment);
        backIndex = 0;
    }

    void shrinkFront() {
        if (segments->GetLength() > 1) {
            delete segments->GetFirst();
            segments->RemoveAt(0)->Instance();
            frontIndex = 0;
        }
    }

    void shrinkBack() {
        if (segments->GetLength() > 1) {
            delete segments->GetLast();
            segments->RemoveAt(segments->GetLength() - 1)->Instance();
            backIndex = segmentSize - 1;
        }
    }

public:
    SegmentedDeque(int segmentSize = 32)
        : segmentSize(segmentSize), frontIndex(0), backIndex(0), size(0) {
        segments = new MutableListSequence<ArraySequence<T>*>();
        segments->Append(new MutableArraySequence<T>(segmentSize));
    }

    SegmentedDeque(const SegmentedDeque& other)
        : segmentSize(other.segmentSize), frontIndex(other.frontIndex),
        backIndex(other.backIndex), size(other.size) {
        segments = new MutableListSequence<ArraySequence<T>*>();
        for (int i = 0; i < other.segments->GetLength(); i++) {
            segments->Append(new MutableArraySequence<T>(*other.segments->Get(i)));
        }
    }

    ~SegmentedDeque() {
        for (int i = 0; i < segments->GetLength(); i++) {
            delete segments->Get(i);
        }
        delete segments;
    }

    void PushFront(T item) {
        if (size == 0) {
            segments->GetFirst()->Set(frontIndex, item);
        }
        else {
            if (frontIndex == 0) {
                expandFront();
            }
            else {
                frontIndex--;
            }
            segments->GetFirst()->Set(frontIndex, item);
        }
        size++;
    }

    void PushBack(T item) {
        if (size == 0) {
            segments->GetLast()->Set(backIndex, item);
        }
        else {
            if (backIndex == segmentSize - 1) {
                expandBack();
            }
            else {
                backIndex++;
            }
            segments->GetLast()->Set(backIndex, item);
        }
        size++;
    }

    T PopFront() {
        if (size == 0) {
            throw std::out_of_range("Deque is empty");
        }

        T item = segments->GetFirst()->Get(frontIndex);
        if (size > 1) {
            if (frontIndex == segmentSize - 1) {
                shrinkFront();
            }
            else {
                frontIndex++;
            }
        }
        size--;
        return item;
    }

    T PopBack() {
        if (size == 0) {
            throw std::out_of_range("Deque is empty");
        }

        T item = segments->GetLast()->Get(backIndex);
        if (size > 1) {
            if (backIndex == 0) {
                shrinkBack();
            }
            else {
                backIndex--;
            }
        }
        size--;
        return item;
    }

    T Front() const {
        if (size == 0) {
            throw std::out_of_range("Deque is empty");
        }
        return segments->GetFirst()->Get(frontIndex);
    }

    T Back() const {
        if (size == 0) {
            throw std::out_of_range("Deque is empty");
        }
        return segments->GetLast()->Get(backIndex);
    }

    T Get(int index) const {
        if (index < 0 || index >= size) {
            throw std::out_of_range("Index out of range");
        }

        int globalIndex = index + frontIndex;
        int segmentNum = globalIndex / segmentSize;
        int localIndex = globalIndex % segmentSize;

        return segments->Get(segmentNum)->Get(localIndex);
    }

    int Size() const {
        return size;
    }

    bool IsEmpty() const {
        return size == 0;
    }

    void SaveToFile(const std::string& filename = "output.txt") const { 
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing");
        }

        file << size << " " << segmentSize << " ";
        for (int i = 0; i < size; i++) {
            file << Get(i) << " ";
        }
        file.close();
    }

    void LoadFromFile(const std::string& filename = "input.txt") { 
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for reading");
        }

        for (int i = 0; i < segments->GetLength(); i++) {
            delete segments->Get(i); 
        }
        delete segments; 
        segments = new MutableListSequence<ArraySequence<T>*>(); 
        size = 0;

        int savedSize, savedSegmentSize;
        file >> savedSize >> savedSegmentSize;

        segmentSize = savedSegmentSize;
        segments->Append(new MutableArraySequence<T>(segmentSize)); 
        frontIndex = 0;
        backIndex = 0;

        T value;
        for (int i = 0; i < savedSize; i++) {
            file >> value;
            PushBack(value); 
        }

        file.close();
    }
    void InsertAndSplit(int dequeIndex, int insertPos, T value) {
        if (dequeIndex < 0 || dequeIndex >= Size()) {
            throw std::out_of_range("Invalid deque index");
        }

        
        MutableArraySequence<T>& target = segments->Get(dequeIndex);

       
        MutableArraySequence<T> newArr(target.GetLength() + 1);
        for (int i = 0; i < insertPos; ++i) {
            newArr.Set(i, target.Get(i));
        }
        newArr.Set(insertPos, value);
        for (int i = insertPos; i < target.GetLength(); ++i) {
            newArr.Set(i + 1, target.Get(i));
        }

        
        auto [first, second] = newArr.SplitAt(insertPos + 1);

        
        segments->RemoveAt(dequeIndex);
        segments->InsertAt(first, dequeIndex);
        segments->InsertAt(second, dequeIndex + 1);
        size++;
    }
};

#endif