#pragma once

#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

namespace Routing {

    namespace Probability {

        class CSVReader {
        public:
            CSVReader(char separator = ';') : m_separator(separator) {}

            std::string const &operator[](std::size_t index) const;

            std::size_t size() const;

            void readNextRow(std::istream &str);

            friend std::istream &operator>>(std::istream &str, CSVReader &data) {
                data.readNextRow(str);
                return str;
            }

        private:
            std::vector<std::string> m_data;
            char m_separator;
        };
    }
}
