#include "PipelineConfig.h"
#include "../Data/Utility/FilesystemUtil.h"
#include <inih/INIReader.h>

#include <sstream>

Routing::Pipeline::PipelineConfig::PipelineConfig(std::string configFile) : configFile(configFile) {
    INIReader reader(configFile);

    if (reader.ParseError() < 0) {
        throw Routing::Exception::PipelineException("Cannot parse config file: " + configFile);
    }

    this->routingIndexPath = reader.Get("data", "routing_index_path", "");
    this->speedProfilesPath = reader.Get("data", "speed_profiles_path", "");
    this->writeResultsCSV = reader.GetBoolean("data", "write_results_csv", 0);
    this->writeResultsSQLite = reader.GetBoolean("data", "write_results_sqlite", 0);

    this->routingAlgorithm = static_cast<PipelineConfig::RoutingAlgorithm>(reader.GetInteger("parameters",
                                                                                             "routing_algorithm", -1));
    this->alternativeCount = reader.GetInteger("parameters", "alternative_count", -1);

    this->ptdr = reader.GetBoolean("parameters", "ptdr", false);
    this->samples = reader.GetInteger("parameters", "ptdr_samples", 100);
    this->loadProfiles = reader.GetBoolean("parameters", "load_profiles", false);

    this->day = static_cast<char>(reader.GetInteger("parameters", "day", 0));
    this->hour = static_cast<char>(reader.GetInteger("parameters", "hour", 0));
    this->minute = static_cast<char>(reader.GetInteger("parameters", "minute", 0));
    this->weekSeconds = Routing::Probability::Utility::GetWeekSeconds(day, hour, minute,
                                                                      0); // Convert departure time to seconds in week, omit seconds

    this->reorderingType = static_cast<PipelineConfig::ReorderingType>(reader.GetInteger("parameters",
                                                                                         "reordering_type", 1));

    std::string msgs = CheckConfig();
    if (!msgs.empty()) {
        throw Exception::PipelineException(msgs);
    }
}

std::string Routing::Pipeline::PipelineConfig::CheckConfig() {
    std::stringstream msgs;

    if (!Utils::CheckFileExists(this->routingIndexPath)) {
        msgs << "Cannot open routing index at: " << this->routingIndexPath << std::endl;
    }

    if (!Utils::CheckFileExists(this->speedProfilesPath)) {
        msgs << "Cannot open speed profiles at: " << this->speedProfilesPath << std::endl;
    }

    int raVal = static_cast<int>(this->routingAlgorithm);
    if (raVal < 1 || raVal > 8) {
        msgs << "Invalid routing algorithm value: " << raVal << std::endl;
    }

    int reVal = static_cast<int>(this->reorderingType);
    if (reVal < 1 || reVal > 2) {
        msgs << "Invalid reordering type value: " << reVal << std::endl;
    }

    if (this->alternativeCount < 1) {
        msgs << "Invalid alternative count: " << this->alternativeCount << std::endl;
    }

    if (this->samples < 1) {
        msgs << "Invalid PTDR sample count: " << this->samples << std::endl;
    }

    if ((day < 0 || day > 6) || (hour < 0 || hour > 23) || (minute < 0 || minute > 59)) {
        msgs << "Invalid departure time: D: " << this->day << " H:" << this->hour << " M:" << this->minute << std::endl;
    }

    if (weekSeconds < 0 || weekSeconds > 604800) {
        msgs << "Invalid week seconds: " << this->weekSeconds << std::endl;
    }

    return msgs.str();
}
