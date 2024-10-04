
#include "ProfileStorageBase.h"

Routing::Data::Probability::ProfileStorageBase::ProfileStorageBase(const std::string path, const bool inMemory) :
        path(path), inMemory(inMemory) {
    this->intervalLength = 0;
    this->intervalCount = 0;
    this->segmentCount = 0;
}


unsigned short Routing::Data::Probability::ProfileStorageBase::GetIntervalLength() const {
    return intervalLength;
}

unsigned short Routing::Data::Probability::ProfileStorageBase::GetIntervalCount() const {
    return intervalCount;
}

unsigned int Routing::Data::Probability::ProfileStorageBase::GetSegmentCount() const {
    return segmentCount;
}

unsigned short Routing::Data::Probability::ProfileStorageBase::GetProfileCount() const {
    return profileCount;
}

const std::string &Routing::Data::Probability::ProfileStorageBase::GetVersion() const {
    return version;
}

std::shared_ptr<Routing::Probability::ProbabilityProfile>
Routing::Data::Probability::ProfileStorageBase::GetProbabilityProfileByEdgeId(const long edgeId) {
    if (inMemory) {
        return LoadFromMemory<Routing::Probability::ProbabilityProfile>(edgeId, this->probabilityProfiles);
    }

    return LoadProbabilityProfileFromStorage(edgeId);
}

std::shared_ptr<std::vector<float>> Routing::Data::Probability::ProfileStorageBase::GetSpeedProfileByEdgeId(
        const long edgeId) {
    if (inMemory) {
        return LoadFromMemory<std::vector<float>>(edgeId, this->speedProfiles);
    }
    return LoadSpeedProfileFromStorage(edgeId);
}

template<typename T>
std::shared_ptr<T> Routing::Data::Probability::ProfileStorageBase::LoadFromMemory(const long edgeId,
                                                                                  const std::map<long, std::shared_ptr<T>> &storage) {
    if (storage.find(edgeId) != storage.end()) {
        return storage.at(edgeId);
    } else {
        return nullptr;
    }
}

long Routing::Data::Probability::ProfileStorageBase::GetDefaultSegmentId() const {
    return defaultSegmentId;
}
