#pragma once

#include <exception>
#include <string>

namespace Routing {

    namespace Exception {

        class NodeNotFoundException : public std::exception {
        public:
            NodeNotFoundException(int nodeId) :
                    nodeId(nodeId) {};

            virtual const char *what() const noexcept {
                const char *ret = "Node Id not found: ";
                return ret;
            }

            int nodeId;
        };

        class ProfileNotFound : public std::exception {
        public:
            ProfileNotFound(int id) :
                    id(id) {};

            virtual const char *what() const noexcept {
                const char *ret = "Profile ID not found\n";
                return ret;
            }

            int id;
        };

        class PipelineException : public std::exception {

        public:
            PipelineException(std::string msg) : msg(msg) {};

            virtual const char *what() const throw() {
                return "PipelineException";
            }

            std::string msg;
        };

        class EdgeNotFoundException : public std::exception {
        public:
            EdgeNotFoundException(int id) :
                    id(id) {};

            virtual const char *what() const throw() {
                return std::string("EdgeId " + std::to_string(this->id) + " not found\n").c_str();
            }

        private:
            int id;
        };
    }

}
