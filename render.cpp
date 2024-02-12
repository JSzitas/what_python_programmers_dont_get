// Tiny QR solver, header only library
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
//
// Copyright (C) 2023- Juraj Szitas
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
namespace fs = std::filesystem;

std::pair<std::string, std::string> tokenize(std::string const &str) {
  size_t pos = str.find(':');
  if(pos == std::string::npos) {
    return {str, ""};
  }
  std::string key = str.substr(0, pos);
  std::string value = str.substr(pos + 2);

  // Remove leading and trailing whitespaces
  key.erase(0, key.find_first_not_of(' '));       // prefixing spaces
  key.erase(key.find_last_not_of(' ')+1);         // suffixing spaces
  value.erase(0, value.find_first_not_of(' '));   // prefixing spaces
  value.erase(value.find_last_not_of(' ')+1);     // suffixing spaces

  return {key, value};
}

int main(int argc, char** argv) {
  std::ofstream output_file(argv[2], std::ios::out | std::ios::app);
  bool header_written = false;
  try {
    for (const auto& entry : std::filesystem::directory_iterator(argv[1])) {
      std::ifstream input_file(entry.path(), std::ios::in);

      if (input_file.is_open()) {

        std::string line;
        std::vector<std::string> headers;
        std::vector<std::string> values;
        while (std::getline(input_file, line)) {
          std::istringstream ss(line);
          std::string token;
          while(std::getline(ss, token, '|')) {
            auto p = tokenize(token);
            headers.push_back(p.first);
            values.push_back(p.second);
          }
        }

        if(!header_written) {
          output_file << "Test, ";
          for(size_t i = 0; i < headers.size(); ++i) {
            output_file << headers[i];
            if(i != headers.size() - 1)
              output_file << ", ";
          }
          output_file << "\n";
          header_written = true;
        }
        output_file << entry.path().filename() << ","; // Write the name of the file

        for(size_t i = 0; i < values.size(); ++i) {
          output_file << values[i];
          if(i != values.size() - 1)
            output_file << ", ";
        }
        output_file << "\n";
        input_file.close();
      }
    }
  } catch (std::filesystem::filesystem_error& e) {
    std::cout << "Error: " << e.what() << '\n';
  }
  output_file.close();
  return 0;
}