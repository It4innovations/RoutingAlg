#pragma once

#include <memory>
#include <vector>
#include <ostream>
#include <algorithm>
#include <utility>
#include <iostream>

using namespace std;

namespace Routing {
    namespace Collections {
        template<class TKey, class TValue>
        class BinaryHeap {
        private:
            const int defaultSize = 8192;
            std::pair<TKey, TValue> *data;
            int count = 0;
            int capacity;

        public:
            BinaryHeap() {
                capacity = defaultSize;
                data = new std::pair<TKey, TValue>[defaultSize];
            }

            ~BinaryHeap() {
                delete[] data;
            }

            int Count() {
                return count;
            }

            int GetCapacity() {
                return capacity;
            }

            void SetCapacity(int value) {
                int previousCapacity = capacity;
                capacity = std::max(value, count);

                if (capacity != previousCapacity) {
                    std::pair<TKey, TValue> *resize = new std::pair<TKey, TValue>[capacity * 2];
                    std::copy(data, data + count, resize);
                    delete[] data;
                    data = resize;
                }
            }

            void Add(TKey priority, TValue value) {
                if (count == capacity) {
                    SetCapacity(capacity * 4);
                }

                data[count] = std::pair<TKey, TValue>(priority, value);
                UpHeap(count);
                count++;
            }

            std::pair<TKey, TValue> Peek() {
                return data[0];
            }

            TKey PeekKey() {
                return data[0].first;
            }

            TValue PeekValue() {
                return data[0].second;
            }

            TValue Remove() {
                if (count == 0) {
                    throw;
                }

                std::pair<TKey, TValue> value = data[0];
                count--;

                data[0] = data[count];
                DownHeap(0);

                return value.second;
            }

            bool Remove(TValue item) {
                bool removed = false;
                int index = -1;

                for (int i = 0; i < count; i++) {
                    if (data[i].second == item) {
                        index = i;
                        break;
                    }
                }

                if (index > 0) {
                    for (int i = index; i < count - 1; i++) {
                        data[i] = data[i + 1];
                    }
                    count--;
                    removed = true;
                }
                return removed;
            }

            bool UpdatePriority(TKey newPriority, TValue item) {
                bool updated = false;
                int index = -1;

                for (int i = 0; i < count; i++) {
                    if (data[i].second == item) {
                        index = i;
                        break;
                    }
                }

                if (index >= 0 && data[index].first > newPriority) {
                    data[index].first = newPriority;
                    UpHeap(index);
                    updated = true;
                }

                return updated;
            }

            void Print(int limit) {
                cout.precision(10);

                if (limit > this->Count()) {
                    limit = this->Count();
                }

                for (int i = 0; i < limit; ++i) {
                    cout << this->counter << " " << data[i].first << " " << data[i].second << endl;
                }

                cout << endl;
            }

        private:
            void UpHeap(int index) {
                int p = index;
                int par = Parent(p);
                std::pair<TKey, TValue> actualData = data[p];

                while (par > -1 && actualData.first < data[par].first) {
                    data[p] = data[par];
                    p = par;
                    par = Parent(p);
                }

                data[p] = actualData;
            }

            void DownHeap(int index) {
                int n;
                int p = index;
                std::pair<TKey, TValue> actualData = data[p];

                while (true) {
                    int ch1 = Child1(p);

                    if (ch1 >= count) {
                        break;
                    }

                    int ch2 = Child2(p);

                    if (ch2 >= count) {
                        n = ch1;
                    } else {
                        n = data[ch1].first < data[ch2].first ? ch1 : ch2;
                    }

                    if (actualData.first > data[n].first) {
                        data[p] = data[n];
                        p = n;
                    } else {
                        break;
                    }
                }

                data[p] = actualData;
            }

            static int Parent(int index) {
                return (index - 1) >> 1;
            }

            static int Child1(int index) {
                return (index << 1) + 1;
            }

            static int Child2(int index) {
                return (index << 1) + 2;
            }
        };
    }
}

