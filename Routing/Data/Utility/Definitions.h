#pragma once

#include <sys/types.h>
#include <cstdint>
#include "../../../include/sparsehash/dense_hash_map"
#include "../../../include/sparsehash/dense_hash_set"
#include "../../Algorithms/Common/VisitedNode.h"
#include "../../Data/Collections/BinaryHeap.h"


#if defined(__GNUC__) || defined(__ICL) || defined(__clang__)
#define expect(x, y) (__builtin_expect((x),(y)))
#else
#define expect(x, y) (x)
#endif

#define likely(x) (expect(x, 1))
#define unlikely(x) (expect(x, 0))

typedef google::dense_hash_map<int, Routing::Algorithms::VisitedNode> VisitedNodeHashMap;
typedef google::dense_hash_set<int> HashSet;
typedef Routing::Collections::BinaryHeap<float, int> BinHeap;
