#pragma once

#include <ostream>

namespace Routing {

    template<typename T>
    struct PosData {
        float x;
        float y;
        //TODO pointer
        T data;

        PosData() {};

        PosData(float x, float y, T &data) :
                x(x),
                y(y),
                data(data) {};

        PosData(float x, float y, T &&data) :
                x(x),
                y(y),
                data(std::move(data)) {};

        PosData(const PosData &other) :
                x(other.x),
                y(other.y),
                data(other.data) {};

        PosData(PosData &&other) :
                x(other.x),
                y(other.y),
                data(std::move(other.data)) {};


        std::ostream &operator<<(std::ostream &os) {
            os << "X: " << this->x << ", Y: " << this->y << ", " << data;
            return os;
        }

        inline int CompareTo(const PosData<T> &other) { return static_cast<int>((this->x - other.x) * COMP_SIZE); }

        bool operator==(const PosData<T> &other) { return this->x == other.x && this->y == other.y; };

        PosData<T> &operator=(PosData<T> &other) {
            x = other.x;
            y = other.y;
            data = other.data;
            return *this;
        }

        PosData<T> &operator=(PosData<T> &&other) {
            x = std::move(other.x);
            y = std::move(other.y);
            data = std::move(other.data);
            return *this;
        }

    private:
        static const int COMP_SIZE = 1000000;
    };
}


