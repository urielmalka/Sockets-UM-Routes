#ifndef SOCKUMFILE_HPP
#define SOCKUMFILE_HPP

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

std::uintmax_t get_file_size(std::string path)
{
    try {
        std::uintmax_t size = fs::file_size(path);
        std::cout << "File size: " << size << " bytes" << std::endl;
        return size;
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}


std::string readChunkFromFile(const std::string& filename, size_t chunkIndex, size_t chunkSize = 204800) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    std::streampos offset = static_cast<std::streampos>(chunkIndex) * chunkSize;
    file.seekg(offset);

    std::string buffer(chunkSize, '\0');
    file.read(&buffer[0], chunkSize);

    buffer.resize(file.gcount());

    return buffer;
}

void appendChunkToFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename, std::ios::binary | std::ios::app); // פתיחה במצב בינארי + append
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for appending: " + filename);
    }

    file.write(content.c_str(), content.size());
    file.close();
}


#endif