#include "SegmentedDeque.h"
#include <fstream>
#include <iostream>

using namespace std;

int main() {
   
    SegmentedDeque<MutableArraySequence<int>> deque;

    
    ifstream input("input.txt");
    if (!input.is_open()) {
        cerr << "�� ������� ������� input.txt" << endl;
        return 1;
    }

    int numArrays;
    input >> numArrays;

    for (int i = 0; i < numArrays; ++i) {
        int size;
        input >> size;
        MutableArraySequence<int> arr(size);
        for (int j = 0; j < size; ++j) {
            int val;
            input >> val;
            arr.Set(j, val);
        }
        deque.PushBack(arr);
    }
    input.close();

    
    ofstream output("output.txt");
    if (!output.is_open()) {
        cerr << "�� ������� ������� output.txt" << endl;
        return 1;
    }

    output << "���������� �������� � ����: " << deque.Size() << endl;
    for (int i = 0; i < deque.Size(); ++i) {
        
        const MutableArraySequence<int>& arr = deque.Get(i);
        output << "������ " << i + 1 << ": ����� " << arr.GetLength() << ", ��������: ";
        for (int j = 0; j < arr.GetLength(); ++j) {
            output << arr.Get(j) << " ";
        }
        output << endl;
    }
    output.close();

    return 0;
}