
#include "ProfileStorageHDF5.h"
#include <iostream>

Routing::Data::Probability::ProfileStorageHDF5::ProfileStorageHDF5(const std::string &path, const bool inMemory)
        : ProfileStorageBase(path, inMemory) {

    H5::Exception::dontPrint(); // Disable annoying stderr messages

    try {
        H5::H5File::isHdf5(path); // Check if this is a valid HDF5 file
        this->h5file.openFile(path, H5F_ACC_RDONLY); // Open profile storage file

        // Read file attributes

        H5::Attribute attrt = this->h5file.openAttribute("TimeStep");
        attrt.read(H5::PredType::NATIVE_USHORT, &this->intervalLength);

        H5::Attribute attrs = this->h5file.openAttribute("IntervalsPerSegment");
        attrs.read(H5::PredType::NATIVE_USHORT, &this->intervalCount);

        H5::Attribute attrv = this->h5file.openAttribute("Version");
        H5::StrType strdatatype = attrv.getStrType();
        attrv.read(strdatatype, this->version);

        H5::Attribute attrn = this->h5file.openAttribute("NumberOfProfiles");
        attrn.read(H5::PredType::NATIVE_USHORT, &this->profileCount);

        // Get number of segments in file
        this->segmentCount = this->h5file.openGroup("/").getNumObjs();

        // List segments in files
        this->defaultSegmentId = this->ListSegments()[0];

#ifndef NDEBUG
        std::cout << "Opened HDF5 storage with " << this->segmentCount << " segments." << std::endl;
        std::cout << "Number of profiles (LoS): " << this->profileCount << std::endl;
        std::cout << "Interval length (s): " << this->intervalLength << std::endl;
        std::cout << "Intervals per segment: " << this->intervalCount << std::endl;
        std::cout << "Version: " << this->version << std::endl;
#endif
    }
    catch (H5::FileIException &e) {
        std::cerr << "Cannot open file " << path << " HDF5 error: " << e.getDetailMsg() << std::endl;
    }
    catch (H5::AttributeIException &e) {
        std::cerr << "Cannot read attribute - HDF5 error: " << e.getDetailMsg() << std::endl;
    }

    if (inMemory) {
        this->LoadProfiles();
    }
}

std::shared_ptr<Routing::Probability::ProbabilityProfile>
Routing::Data::Probability::ProfileStorageHDF5::LoadProbabilityProfileFromStorage(const long edgeId) {
    std::lock_guard<std::mutex> lock(this->h5file_mutex);

    H5::DataSet dataset;
    std::string grpName = std::to_string(edgeId);
    try {
        this->h5file.openGroup(grpName);
    }
    catch (H5::FileIException &fe) {
        return nullptr;
    }
    catch (H5::GroupIException &ge) {
        return nullptr;
    }


    H5::DataSet speedProbabilityDataset = this->h5file.openGroup(grpName).openDataSet(
            PROBABILITY_DATASET_NAME);
    std::vector<std::vector<Routing::Probability::SpeedProbability>> speedProbability = LoadSpeedProbability(
            speedProbabilityDataset);

    H5::DataSet aliasDataset = this->h5file.openGroup(std::to_string(edgeId)).openDataSet(ALIAS_DATASET_NAME);
    std::vector<int> alias = LoadFlatDataset<int>(aliasDataset, this->profileCount, H5::PredType::NATIVE_INT32);

    H5::DataSet probDataset = this->h5file.openGroup(std::to_string(edgeId)).openDataSet(PROB_ALIAS_DATASET_NAME);
    std::vector<float> prob = LoadFlatDataset<float>(probDataset, this->profileCount, H5::PredType::NATIVE_FLOAT);

    return std::shared_ptr<Routing::Probability::ProbabilityProfile>(
            new Routing::Probability::ProbabilityProfile(speedProbability, alias, prob, this->intervalLength));


}

std::vector<std::vector<Routing::Probability::SpeedProbability>>
Routing::Data::Probability::ProfileStorageHDF5::LoadSpeedProbability(const H5::DataSet &dataset) {
    std::vector<std::vector<Routing::Probability::SpeedProbability>> convertedProfiles;
    std::vector<double> rawProfile(this->profileCount * 2 * this->intervalCount); // Raw dataset TODO: refactor

    try {
        H5::DataSpace dataspace = dataset.getSpace();
        int rank = dataspace.getSimpleExtentNdims();
        if (rank != 2) {
            std::cerr << "Dataset: " << dataset.getObjName() << " invalid number of dimensions (" << rank << ")"
                      << std::endl;
        }

        hsize_t dimsize[2];
        dataspace.getSimpleExtentDims(dimsize);

        if (dimsize[0] != this->intervalCount || dimsize[1] != 2 * this->profileCount) {
            std::cerr << "Dataset: " << dataset.getObjName() << " has invalid size." << std::endl;
        }

        dataset.read(rawProfile.data(), H5::PredType::NATIVE_DOUBLE);

        for (int i = 0; i < this->intervalCount; ++i) {
            std::vector<Routing::Probability::SpeedProbability> singleProfile;
            for (int p = 0; p < this->profileCount * 2; ++p) {
                float speed = rawProfile[(i * this->profileCount * 2) + p] *
                              oneDiv3point6; // Load speed and convert from km/h to m/s
                float probability = rawProfile[(i * this->profileCount * 2) + ++p];
                singleProfile.emplace_back(speed, probability);
            }
            convertedProfiles.push_back(singleProfile);
        }
    }
    catch (H5::DataSetIException &e) {
        std::cerr << "Cannot read dataset " << dataset.getObjName() << " error: " << e.getDetailMsg() << std::endl;
    }

    return convertedProfiles;
}

herr_t Routing::Data::Probability::GroupIterator(hid_t loc_id, const char *name, void *opdata) {

    try {
        auto output = static_cast<std::vector<long> *>(opdata);
        output->push_back(std::stol(name));
        return 0;
    } catch (const std::invalid_argument &e) {
        std::cerr << "Invalid segment id format: " << name << std::endl;
        return -1;
    } catch (const std::out_of_range &e) {
        std::cerr << "String is not an edge ID: " << name << std::endl;
        return -1;
    }
}

void Routing::Data::Probability::ProfileStorageHDF5::LoadProfiles() {
    // Load profiles
    std::cout << "Loading profiles to memory..." << std::endl;

    long loaded = 0;
    for (const auto &seg : this->ListSegments()) {
        try {
            this->probabilityProfiles.emplace(seg, this->LoadProbabilityProfileFromStorage(seg));
            this->speedProfiles.emplace(seg, this->LoadSpeedProfileFromStorage(seg));
            std::cout << "\r" << ++loaded << "/" << this->GetSegmentCount() << std::flush;
        } catch (std::invalid_argument &e) {
            std::cerr << "Invalid segment id: " << seg << " " << e.what() << std::endl;
            return;
        } catch (std::out_of_range &e) {
            std::cerr << "Segment id out of range: " << seg << " " << e.what() << std::endl;
            return;
        }
    }
    std::cout << std::endl;
}

std::vector<long> Routing::Data::Probability::ProfileStorageHDF5::ListSegments() {
    // List all segment groups in file
    std::vector<long> output;
    this->h5file.iterateElems("/", nullptr, GroupIterator, &output); // Accepts only c-style function ptr, hence void *
    return output;
}

bool Routing::Data::Probability::ProfileStorageHDF5::HasSegment(long segmentId) {
    // Memory storage
    if (inMemory) {
        return this->probabilityProfiles.find(segmentId) != this->probabilityProfiles.end();
    }

    // File storage
    try {
        this->h5file.openGroup(std::to_string(segmentId));
        return true;
    }
    catch (H5::FileIException &fe) {
        return false;
    }
    catch (H5::GroupIException &ge) {
        return false;
    }
}

std::shared_ptr<std::vector<float>> Routing::Data::Probability::ProfileStorageHDF5::LoadSpeedProfileFromStorage(
        const long edgeId) {
    std::string grpName = std::to_string(edgeId);
    try {
        this->h5file.openGroup(grpName);
    }
    catch (H5::FileIException &fe) {
        return nullptr;
    }
    catch (H5::GroupIException &ge) {
        return nullptr;
    }

    H5::DataSet dataset = this->h5file.openGroup(grpName).openDataSet(SPEED_DATASET_NAME);
    std::shared_ptr<std::vector<float>> speedProfile(
            new std::vector<float>(LoadFlatDataset<float>(dataset, 1, H5::PredType::NATIVE_FLOAT)));

    // Convert loaded speed profile from km/h to m/s
    for (size_t i = 0; i < speedProfile->size(); i++) {
        (*speedProfile)[i] *= this->oneDiv3point6;
    }
    return speedProfile;
}

template<typename T>
std::vector<T> Routing::Data::Probability::ProfileStorageHDF5::LoadFlatDataset(const H5::DataSet &dataset, long columns,
                                                                               H5::PredType type) {
    std::vector<T> result(this->intervalCount * columns);
    try {
        H5::DataSpace dataspace = dataset.getSpace();
        int rank = dataspace.getSimpleExtentNdims();
        if (rank != 2) {
            std::cerr << "Dataset: " << dataset.getObjName() << " invalid number of dimensions (" << rank << ")"
                      << std::endl;
        }

        hsize_t dimsize[2];
        dataspace.getSimpleExtentDims(dimsize);

        if (dimsize[0] != static_cast<hsize_t>(this->intervalCount) || dimsize[1] != static_cast<hsize_t>(columns)) {
            std::cerr << "Dataset: " << dataset.getObjName() << " has invalid size." << std::endl;
        }

        dataset.read(result.data(), type);
    }
    catch (H5::DataSetIException &e) {
        std::cerr << "Cannot read dataset " << dataset.getObjName() << " error: " << e.getDetailMsg() << std::endl;
    }

    return result;
}
