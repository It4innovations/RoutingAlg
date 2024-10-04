#include "SessionReordering.h"
#include <set>

Routing::Algorithms::SessionReordering::SessionReordering(const shared_ptr<Routing::Data::GraphMemory> &routingGraph,
                                                          const vector<float> &weights) : ReorderingBase(routingGraph),
                                                                                          weights(weights) {
#ifndef NDEBUG
    std::cout << "Session reordering weights: " << std::endl;
    for (size_t i = 0; i < weights.size(); ++i) {
        std::cout << i << ":" << weights[i] << std::endl;
    }
    std::cout << std::endl;
#endif
}


vector<long long int> Routing::Algorithms::SessionReordering::Reorder(const std::vector<ReorderInput> &reorderInput,
                                                                      std::vector<long long> &lastRoute, const long long int currentTmw, const long long int tmwSize)
{
	// Check minimal amount of routes to reorder
	if(reorderInput.size() < 2)
	{
		std::cerr << "ERROR: Not enough routes to reorder." << std::endl;
        if (reorderInput.size() == 1)
        {
            return {0};
        }
        else return {};
	}

    // Reordering output - map with coefficients, multiple routes can have the same coeff
    std::map<float, std::vector<int>> finalOrder;

    // Iterate over routes
    for (size_t i = 0; i < reorderInput.size(); ++i) {
        // Store edgeIds for segments
        std::vector<long long> edgeIds;
        for (const auto &s : reorderInput[i].segments) {
            edgeIds.push_back(s.first.edgeId);
        }

        // Get TMWs quantized from current time
        auto currentRouteTmw = this->QuantizeTravelTime(edgeIds, currentTmw, tmwSize);

        // Normalized values in time bins
        // Key: Time bin, Value: cars per meter
        std::map<long long, float> normalizedLoad;

		// Segment ID overlap count
		// Increased when current segment is also found in last route
		long overlapCount = 0;

        // Index of time bin in quantized current route
        size_t currentRouteTmwIdx = 0;

        for (const auto &s : reorderInput[i].segments) {
            // Skip segments shorter than threshold
            if (s.first.length < 20.0f) {
                continue;
            }

            // Iterate over time bins with future load
            // TODO: Handle case when currentRouteTmw.size() > s.second.size()
            for (const auto &t : s.second) {
                // Skip past time bins
                if (t.first < currentTmw) {
                    continue;
                }

                /**
                 * Future load of segments
                 */
                float addCurrentSegmentPct = 0.0f;

                if (currentRouteTmwIdx < currentRouteTmw.size()) {
                    // Take current route into account only if it has same amount of tmws
                    for (const auto &rBin : currentRouteTmw[currentRouteTmwIdx].second) {
                        // If current segment appears in current time bin, increase predicted load
                        if (rBin.first == s.first.edgeId) {
                            // Increment current load as pct of segment in current time bin
                            addCurrentSegmentPct += rBin.second;
                        }
                    }
                }

                // Normalize load to cars per meter and aggregate over segments
                float nl = static_cast<float>(t.second + addCurrentSegmentPct) / s.first.length;
                if (normalizedLoad.find(t.first) == normalizedLoad.end()) {
                    normalizedLoad[t.first] = nl;
                } else {
                    normalizedLoad[t.first] += nl;
                }

				/**
				 * Overlap with last route returned
				 */
					// Iterate over segments in last route
					// If current segment is found, increment overlap
					for (const auto& lastSeg : lastRoute)
					{
						// Try to find the segment in last route in current tmw
						if(lastSeg == s.first.edgeId)
						{
							overlapCount++;
							break;
						}
					}
				currentRouteTmwIdx++;
			}
		}


		// Compute overlap as percentage of segments identical to the previous route
		// TODO: Take segment order into account?
		// TODO: Take tmw overlap?
		float routeOverlap = static_cast<float>(overlapCount) / reorderInput[i].segments.size();

        // Find max normalized load over all time bins
        float maxNormalizedLoad = 0.0;
        for (const auto &l : normalizedLoad) {
            if (maxNormalizedLoad < l.second) {
                maxNormalizedLoad = l.second;
            }
        }

        // PTDR coefficient - normalized delay
        //TODO: Version without PTDR?
        float normDelay = reorderInput[i].mean / reorderInput[i].optimalTravelTime;

        // Final ordering coeff
        float orderingCoeff = (this->weights[0] * (1.0f - routeOverlap) + this->weights[1] * maxNormalizedLoad +
                               this->weights[2] * normDelay) / 3.0f;

#ifndef NDEBUG
        std::cout << "Overlap: " << routeOverlap << " Max load: " << maxNormalizedLoad << " Delay: " << normDelay
                  << " (coeff: " << orderingCoeff << ")" << std::endl;
#endif

        // Insert coeff along with route idx to map
        finalOrder[orderingCoeff].push_back(i);
    }

    std::vector<long long int> result;
    for (const auto &ord : finalOrder) {
        for (const auto &rte : ord.second) {
#ifndef NDEBUG
            std::cout << "Route: " << rte << " coeff: " << ord.first << std::endl;
#endif
            result.push_back(rte);
        }
    }

    return result;
}
