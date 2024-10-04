#pragma once

#include <H5Cpp.h>

namespace Routing {

    class DataIndexBase {
    public:
        DataIndexBase(const H5::H5File &h5file) : h5file(h5file) {}

    protected:
        const H5::H5File h5file;
    };

}



