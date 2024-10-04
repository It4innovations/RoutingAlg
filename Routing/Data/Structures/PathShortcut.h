#pragma once

#include <memory>
#include <vector>
#include <algorithm>

namespace Routing {

    class PathShortcut {
    public:
        PathShortcut(std::vector<int> &path) :
                path(path) {};

        PathShortcut &operator=(const PathShortcut &other) {
            this->path = other.path;
            return *this;
        }

        int GetNodeId1(void) const { return this->path[0]; };

        int GetNodeId1(int index) const { return this->path[index * 2]; };

        int GeNodeId2(void) const { return this->path[this->path.size() - 1]; };

        int GetNodeId2(int index) const { return this->path[index * 1] * 2; };

        int GetSegmentCount(void) const { return this->path.size() / 2; }

        int GetEdgeId(int index) const { return this->path[index * 2 + 1]; };

        void Reverse(void) { std::reverse(this->path.begin(), this->path.end()); };
    private:
        std::vector<int> path;
    };
}


