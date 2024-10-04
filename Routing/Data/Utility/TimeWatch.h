#pragma once

#include <chrono>
#include <iostream>

namespace Routing {

    namespace Utils {

        class TimeWatch {

            typedef std::chrono::high_resolution_clock high_resolution_clock;
            typedef std::chrono::milliseconds milliseconds;

        public:
            explicit TimeWatch(bool start = false) : isStopped(false) {
                if (start) {
                    this->Restart();
                }
            }

            void Start(void) {
                this->Restart();
            };

            void Stop(void) {
                this->isStopped = true;
                auto diff = high_resolution_clock::now() - this->start;
                this->lastInterval = std::chrono::duration_cast<milliseconds>(diff);
            };

            void Restart(void) {
                this->isStopped = false;
                this->start = high_resolution_clock::now();
            };

            long GetElapsed(bool restart = false) {
                if (!isStopped) {
                    this->Stop();
                }

                if (restart) {
                    this->Restart();
                }

                return this->lastInterval.count();
            }

        private:
            bool isStopped = false;
            high_resolution_clock::time_point start;
            milliseconds lastInterval;
        };
    }
}


