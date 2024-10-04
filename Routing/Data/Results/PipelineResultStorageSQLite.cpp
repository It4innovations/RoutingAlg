#include "PipelineResultStorageSQLite.h"

Routing::Data::PipelineResultStorageSQLite::PipelineResultStorageSQLite(const Pipeline::PipelineConfig& pipelineConfig,
                                                                        const std::string& filename)
        : PipelineResultStorageBase(pipelineConfig) {

    int sqlite_ret;
    sqlite_ret = sqlite3_open(filename.c_str(), &db);
    HandleReturn(sqlite_ret);

    // Create file schema
    CreateSchema();

    std::cout << "ResultStorage: Created result file: " << filename << std::endl;
}

Routing::Data::PipelineResultStorageSQLite::~PipelineResultStorageSQLite() {
    if (db != nullptr) {
        int ret = sqlite3_close_v2(db);
        if (ret != SQLITE_OK) {
            std::cerr << "Error closing SQLite3 file!" << std::endl;
        }
    }
}

void Routing::Data::PipelineResultStorageSQLite::HandleReturn(int ret) {
    if (ret != SQLITE_OK) {
        std::cerr << "SQLite error: " << sqlite3_errmsg(db) << std::endl;
    }
}

void Routing::Data::PipelineResultStorageSQLite::CreateSchema() {

    ExecStatement(
            "CREATE TABLE requests ("
            "request_id PRIMARY KEY,"
            "origin INTEGER,"
            "destination INTEGER);");

    ExecStatement(
            "CREATE TABLE routes ("
            "request_id INTEGER,"
            "route_id INTEGER,"
            "edge_gid INTEGER,"
            "FOREIGN KEY (request_id) REFERENCES requests(request_id));"
    );

    ExecStatement("CREATE INDEX routes_request ON routes (request_id);");
    ExecStatement("CREATE INDEX routes_route_request ON routes (route_id, request_id);");
    ExecStatement("CREATE INDEX routes_route ON routes (route_id);");

}

void Routing::Data::PipelineResultStorageSQLite::ExecStatement(std::string stmt) {
    char *errmsg;
    int ret = sqlite3_exec(db, stmt.c_str(), nullptr, nullptr, &errmsg);
    if (ret != SQLITE_OK) {
        std::cerr << "SQLite error: " << errmsg << std::endl;
        sqlite3_free(errmsg);
    }
}

void Routing::Data::PipelineResultStorageSQLite::StoreResult(const Routing::Pipeline::RoutingRequest &request) {
    sqlite3_stmt *insert;
    int ret = sqlite3_prepare_v2(db, "INSERT INTO requests VALUES (?,?,?)", -1, &insert, nullptr);
    HandleReturn(ret);

    char* errorMessage;
    ret = sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, &errorMessage);
    HandleReturn(ret);

    // Bind vals
    ret = sqlite3_bind_int(insert, 1, request.id);
    HandleReturn(ret);

    ret = sqlite3_bind_int(insert, 2, request.origin);
    HandleReturn(ret);

    ret = sqlite3_bind_int(insert, 3, request.destination);
    HandleReturn(ret);

    // Execute INSERT
    ret = sqlite3_step(insert);
    HandleStepReturn(ret);

    ret = sqlite3_finalize(insert);
    HandleReturn(ret);

    ret = sqlite3_prepare_v2(db, "INSERT INTO routes VALUES (?,?,?)", -1, &insert, nullptr);
    HandleReturn(ret);

    for (size_t i = 0; i < request.routes.size(); ++i) {
        for (size_t seg = 0; seg < request.routes[i].size(); ++seg) {

            // Request ID
            ret = sqlite3_bind_int(insert, 1, request.id);
            HandleReturn(ret);

            // Route ID
            ret = sqlite3_bind_int(insert, 2, i);
            HandleReturn(ret);

            // Edge GID
            ret = sqlite3_bind_int(insert, 3, request.routes[i][seg].edgeId);
            HandleReturn(ret);

            ret = sqlite3_step(insert);
            HandleStepReturn(ret);

            ret = sqlite3_reset(insert);
            HandleReturn(ret);
        }
    }

    ret = sqlite3_finalize(insert);
    HandleReturn(ret);

    ret = sqlite3_exec(db, "COMMIT TRANSACTION", nullptr, nullptr, &errorMessage);
    HandleReturn(ret);
}

void Routing::Data::PipelineResultStorageSQLite::HandleStepReturn(int ret) {
    if (ret != SQLITE_DONE) {
        std::cerr << "SQLite error: " << sqlite3_errmsg(db) << std::endl;
    }
}
