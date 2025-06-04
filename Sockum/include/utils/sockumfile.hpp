/**
 * @author Uriel Malka - urielmalka1@gmail.com
 * @brief File utility functions for chunk-based file transfer.
 * 
 * This header provides utility functions to support file reading and writing
 * in fixed-size chunks, which is useful for sending files over a network in segments.
 * It leverages C++17's <filesystem> and standard I/O streams.
 * 
 * Core functionalities:
 * - `get_file_size`: Retrieves and prints the size of a file in bytes.
 * - `readChunkFromFile`: Reads a specific chunk from a file based on chunk index and size.
 * - `appendChunkToFile`: Appends binary data to the end of a file.
 * 
 * These utilities are designed to support the Sockum system's file routing and transfer mechanisms.
 */


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


/**
 * @brief Reads a specific chunk from a binary file.
 * @param filename The path to the file.
 * @param chunkIndex The zero-based index of the chunk to read.
 * @param chunkSize The size of each chunk in bytes (default is 200 KB).
 * @return A string containing the read bytes (may be smaller than chunkSize at EOF).
 * @throws std::runtime_error if the file cannot be opened.
 */

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