#pragma once

#include "memory"

#include "GpsPoint.h"

namespace Routing {

    class GpsRectangle {
    public:
        GpsRectangle(void) :
                topLeft(new GpsPoint(std::numeric_limits<float>::min(), std::numeric_limits<float>::max())),
                bottomRight(new GpsPoint(std::numeric_limits<float>::max(), std::numeric_limits<float>::min())) {};

        GpsRectangle(GpsPoint &p1, GpsPoint &p2) :
                topLeft(new GpsPoint(std::move(p1))),
                bottomRight(new GpsPoint(std::move(p2))) {};

        GpsRectangle(float left, float top, float right, float bottom) :
                topLeft(new GpsPoint(top, left)),
                bottomRight(new GpsPoint(bottom, right)) {};

        GpsRectangle(const GpsRectangle &other) :
                topLeft(new GpsPoint(*other.topLeft)),
                bottomRight(new GpsPoint(*other.bottomRight)) {};

        GpsRectangle(GpsRectangle &&other) :
                topLeft(std::move(other.topLeft)),
                bottomRight(std::move(other.bottomRight)) {
            other.topLeft = nullptr;
            other.bottomRight = nullptr;
        };

        GpsRectangle &operator=(const GpsRectangle &other) {
            this->topLeft.reset(new GpsPoint(*other.topLeft));
            this->bottomRight.reset(new GpsPoint(*other.bottomRight));

            return *this;
        }

        void Expand(float lat, float lon);

        void Expand(const GpsPoint &point);

        void Expand(const GpsRectangle &rect);

        void MultiplySize(float scale);


        std::ostream &operator<<(std::ostream &os) {
            os.precision(4);
            os << this->topLeft->lon << " " << this->topLeft->lat << " " <<
               this->bottomRight->lon << " " << this->bottomRight->lat;

            return os;
        }

        inline bool IsIn(float lat, float lon) {
            return !(lon < this->topLeft->lon || lon > this->bottomRight->lon
                     || lat < this->bottomRight->lat || lat > this->topLeft->lat);
        };

        inline GpsPoint GetBottomRight(void) { return *this->bottomRight; };

        inline GpsPoint GetTopLeft(void) { return *this->topLeft; };

        inline float GetLeft(void) { return this->topLeft->lon; };

        inline float GetTop(void) { return this->topLeft->lat; };

        inline float GetRight(void) { return this->bottomRight->lon; };

        inline float GetBottom(void) { return this->bottomRight->lat; };

    private:
        std::unique_ptr<GpsPoint> topLeft;
        std::unique_ptr<GpsPoint> bottomRight;
    };
}


