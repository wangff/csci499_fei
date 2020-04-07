#include "persistence.h"

namespace cs499_fei {
// Helper function: Write a number to the file.
void writeNumberToFile(std::ofstream &outfile, size_t size) { outfile << size; }

// Helper function: Write a string to the file.
void writeStringToFile(std::ofstream &outfile, const std::string &str) {
  outfile << str;
}

// Helper function: write a delim '#' to the file.
void writeDelimToFile(std::ofstream &outfile, char delim = '#') {
  outfile.write(&delim, sizeof delim);
}

// Helper function: read a number which will end with a delimiter '#"/
size_t readNumberFromFileUntilDelim(std::ifstream &infile, char delim = '#') {
  std::string sizeStr;
  getline(infile, sizeStr, delim);
  size_t size = stoi(sizeStr);
  return size;
}

// Helper function: read a string with fixed size.
std::string readStringFromFile(std::ifstream &infile, const size_t length) {
  if (length == 0 || infile.bad()) {
    return "";
  }

  // construct string to stream size
  std::string str(length, '\0');
  infile.read(&str[0], length);
  return str;
}

// write key-value map into file.
void Persistence::serialize(const StringKVMap &kv_store,
                            const std::string &to_file) {
  std::ofstream outfile(to_file);

  // write count of pairs to the first line for the later verification.
  writeNumberToFile(outfile, kv_store.size());
  writeDelimToFile(outfile);

  for (const auto &p : kv_store) {
    // for each key-value, since both are strings
    // we write every string to file with format "size#content"
    // size is fixed type.
    std::string key = p.first;
    writeNumberToFile(outfile, key.size());
    writeDelimToFile(outfile);
    writeStringToFile(outfile, key);

    std::string value = p.second;
    writeNumberToFile(outfile, value.size());
    writeDelimToFile(outfile);
    writeStringToFile(outfile, value);
  }
  outfile.close();
}

StringKVMap Persistence::deserialize(const std::string &from_file) {
  bool exist = std::experimental::filesystem::exists(from_file);
  if (!exist) {
    return {};
  }

  StringKVMap ret;

  std::ifstream infile(from_file, std::ios::in);
  if (!infile.is_open()) {
    return {};
  }

  // get length of file:
  infile.seekg(0, infile.end);
  int length = infile.tellg();

  // recover file pointer
  infile.seekg(0, infile.beg);

  // first line to read count of pairs we will read from file.
  size_t size = readNumberFromFileUntilDelim(infile);

  // start reading key value from local file.

  while (infile.tellg() < length) {
    auto key_length = readNumberFromFileUntilDelim(infile);
    auto key = readStringFromFile(infile, key_length);

    auto value_length = readNumberFromFileUntilDelim(infile);
    auto value = readStringFromFile(infile, value_length);
    ret[key] = value;
  }

  infile.close();
  return ret;
}
}  // namespace cs499_fei