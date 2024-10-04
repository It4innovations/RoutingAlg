#include "CSVReader.h"
#include <algorithm>

void Routing::Probability::CSVReader::readNextRow(std::istream &str) {
    std::string line;
    std::getline(str, line);
    if (line.size() == 1 && line[0] == '\n') {
        // empty line
        return;
    }

    std::string cell;
    std::stringstream lineStream(line);
    m_data.clear();
    while (std::getline(lineStream, cell, m_separator)) {
        // Trim newline characters at the end of the line
        cell.erase(std::remove(cell.begin(), cell.end(), '\n'), cell.end());
        cell.erase(std::remove(cell.begin(), cell.end(), '\r'), cell.end());
        cell.erase(std::remove(cell.begin(), cell.end(), ' '), cell.end());

        m_data.push_back(cell);
    }
}

std::size_t Routing::Probability::CSVReader::size() const {
    return m_data.size();
}

std::string const &Routing::Probability::CSVReader::operator[](std::size_t index) const {
    return m_data[index];
}