#pragma once

#include "../../Data/Exceptions/Exceptions.h"
#include "../../Data/Indexes/GraphMemory.h"
#include "../../Data/Probability/ProfileStorageHDF5.h"
#include "../Base/Algorithm.h"

namespace Routing {

    namespace Algorithms {

        namespace TD {

            class TDBase : protected Algorithm {
            public:
                TDBase(std::shared_ptr<GraphMemory> routingGraph,
                       std::shared_ptr<Routing::Data::Probability::ProfileStorageHDF5> storage,
                       AlgorithmSettings settings = AlgorithmSettings()) : Algorithm(routingGraph, settings),
                                                                           storage(storage) {

                };

            protected:
                std::shared_ptr<Routing::Data::Probability::ProfileStorageHDF5> storage;

            };
        }
    }
}


