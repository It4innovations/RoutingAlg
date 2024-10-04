#pragma once

#include <iostream>
#include "../../Data/Structures/Node.h"

using Routing::Node;

namespace Routing {

    namespace Algorithms {

        class GraphFilterBase {
        protected:
            GraphFilterBase *nextFilter = nullptr;

            virtual void SetOwnData(const Node &destinationNode, Edge &edge) = 0;

        public:
            GraphFilterBase() {
                nextFilter = nullptr;
            }

            virtual void DeleteAllFilter() = 0;

            void SetAllFiltersData(const Node &destinationNode, Edge &edge) {
                SetOwnData(destinationNode, edge);

                if (nextFilter != nullptr) {
                    nextFilter->SetAllFiltersData(destinationNode, edge);
                }
            }

            void AddFilter(GraphFilterBase *nextFilter) {
                if (this->nextFilter != nullptr) {
                    this->nextFilter->AddFilter(nextFilter);
                } else {
                    this->nextFilter = nextFilter;
                }
            }

            virtual void Handle(bool &filtersPass) = 0;

            virtual ~GraphFilterBase() = default;

        };
    }
}

