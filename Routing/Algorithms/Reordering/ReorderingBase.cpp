#include "ReorderingBase.h"

Routing::Algorithms::ReorderingBase::ReorderingBase(const std::shared_ptr<Routing::Data::GraphMemory> &routingGraph)
        : routingGraph(routingGraph) {}

std::vector<std::pair<long long, std::vector<std::pair<long long, float>>>>
Routing::Algorithms::ReorderingBase::QuantizeTravelTime(const std::vector<long long> &route, const int currentTmw,
                                                        const int tmwSize) {
    /* Original version */
    std::vector<std::pair<long long, std::vector<std::pair<long long, float>>>> result;

    std::pair<long long, std::vector<std::pair<long long, float>>> curr_time_gid = {currentTmw,
                                                                                    std::vector<std::pair<long long, float>>()};

    float mps = 0;
    float time_remains = 0;
    float curr_speed = 0;
    float curr_lenght = 0;
    float add_time = 0;
    float add_current = false;

    int currentTravelTmw = currentTmw;

    auto get_ln_time = [&](float lng) {
        return lng / mps;
    };

    auto get_run_percent = [](float mps, long run_time, float total_ln) -> float {
        return (mps * run_time) / total_ln;
    };

    //TODO remove log, double check and add czech commentaries
    for (unsigned int i = 0; i < route.size(); ++i) {
        long edge_id = route[i];
        auto edge = this->routingGraph->GetEdgeById(edge_id);

        curr_speed = edge.GetSpeed();
        curr_lenght = edge.length;

        //meters per second with current speed
        mps = (curr_speed * 5) / 18;

        //kolik se ujede za dany casovy usek
        float curr_tms;

        if (time_remains) {
            curr_tms = time_remains;
        } else {
            curr_tms = tmwSize + add_time;
        }

        auto curr_run_m = mps * curr_tms; // How much distance on t
        add_time = 0;
        auto curr_diff = curr_run_m - curr_lenght;

        //segment se nedojel v danem timestempu
        if (curr_diff <= 0) {
            add_current = false;
            time_remains = 0;
            float current_perc = get_run_percent(mps, tmwSize, curr_lenght);

            curr_time_gid.second.emplace_back(std::make_pair(edge_id, current_perc));

            //kolik je potreba casu k dojeti  segmentu
            add_time = get_ln_time(-curr_diff);
            result.emplace_back(curr_time_gid);

            //usek presult.vic time stepu
            while (add_time > tmwSize) {
                currentTravelTmw += tmwSize;
                add_time -= tmwSize;

                current_perc = get_run_percent(mps, tmwSize, curr_lenght);

                std::vector<std::pair<long long, float>> single_ed = {std::make_pair(edge_id, current_perc)};
                result.emplace_back(std::make_pair(currentTravelTmw, std::move(single_ed)));
            }

            if (add_time > 0 && i == route.size() - 1) {
                add_current = true;
            }

            current_perc = get_run_percent(mps, add_time, curr_lenght);
            // TODO if add_time = 0 then set curr_time_gid to empty
            currentTravelTmw += tmwSize;
            std::vector<std::pair<long long, float>> curr_edge_arr = {std::make_pair(edge_id, current_perc)};
            curr_time_gid = {currentTravelTmw, std::move(curr_edge_arr)};
        }
            //segment se dojel v danem timestampu
        else {
            add_current = true;

            if (time_remains) {
                time_remains -= tmwSize - get_ln_time(curr_lenght);
            } else {
                time_remains = tmwSize - get_ln_time(curr_lenght);
            }
            //1.0 - 100% - a whole segment in current time stamp
            curr_time_gid.second.emplace_back(std::make_pair(edge_id, 1.0));
        }
    }

    if (add_current) {
        result.emplace_back(curr_time_gid);
    }

    return result;
}
