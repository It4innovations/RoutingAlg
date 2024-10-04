#include "GraphFilterBase.h"

#pragma once

#define MINFRC 7
#define MAXFRC 0

namespace Routing {

    namespace Algorithms {

        class GraphFilterHierarchy : public GraphFilterBase {

        public:

            GraphFilterHierarchy(int minFRC, int maxFRC) {
                edgeFRC = MAXFRC;

                if (minFRC < maxFRC) {
                    int tmp = minFRC;
                    minFRC = maxFRC;
                    maxFRC = tmp;
                }

                if (minFRC <= MINFRC && minFRC >= MAXFRC) {
                    this->minFRC = minFRC;
                } else {
                    this->minFRC = MINFRC;
                }

                if (maxFRC <= MINFRC && maxFRC >= MAXFRC) {
                    this->maxFRC = maxFRC;
                } else {
                    this->maxFRC = MAXFRC;
                }
            }

            void DeleteAllFilter() final {
                if (nextFilter != nullptr) {
                    nextFilter->DeleteAllFilter();
                }

                delete nextFilter;
            }

            void Handle(bool &filtersPass) final {
                filtersPass = ApplyFilter();

                if (this->nextFilter != nullptr && filtersPass == true) {
                    nextFilter->Handle(filtersPass);
                }
            }

            void SetMinFRC(int minFRC) {
                if (minFRC >= 0 && minFRC <= 7) {
                    this->minFRC = minFRC;
                }
            }

            void SetMaxFRC(int maxFRC) {
                if (maxFRC >= 0 && maxFRC <= 7) {
                    this->maxFRC = maxFRC;
                }
            }

            ~GraphFilterHierarchy() = default;

        private:
            int minFRC;
            int maxFRC;
            int edgeFRC;

            void SetOwnData(const Node &destinationNode, Edge &edge) final {
                this->edgeFRC = (int) edge.GetFuncClass();
            }

            bool ApplyFilter() {
                return (edgeFRC <= minFRC && edgeFRC >= maxFRC);
            }

        };
    }
}


