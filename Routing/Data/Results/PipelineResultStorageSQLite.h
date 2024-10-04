#pragma once

#include "PipelineResultStorageBase.h"
#include <sqlite3.h>

namespace Routing {
    namespace Data {

        class PipelineResultStorageSQLite : public PipelineResultStorageBase {

        public:
            PipelineResultStorageSQLite(const Pipeline::PipelineConfig& pipelineConfig, const std::string& filename);

            ~PipelineResultStorageSQLite();

            void StoreResult(const Pipeline::RoutingRequest &request) override;

        private:
            void HandleReturn(int ret);

            void HandleStepReturn(int ret);

            void CreateSchema();

            void ExecStatement(std::string stmt);

            sqlite3 *db;
        };
    }
}
