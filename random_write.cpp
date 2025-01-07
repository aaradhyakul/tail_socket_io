#include <algorithm>
#include <cassert>
#include <chrono>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <thread>

const int TAIL_LINES = 5;

const std::string log_filename = "log_file.txt";
const std::string random_filename = "random_file.txt";
const std::string tail_log_filename = "tail_log_file.txt";

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> dist_bool(0, 1);
std::uniform_int_distribution<int> dist_int(0, 5);

bool isWhitespaceOrNewline(const std::string &str) {
  return std::all_of(str.begin(), str.end(),
                     [](char ch) { return std::isspace(ch) || ch == '\n'; });
}

void appendToLogfile(const std::string &log_filename,
                     const std::string &random_filename) {
  std::ofstream log_file(log_filename, std::ios::app);
  std::ifstream random_file(random_filename);
  if (!log_file) {
    std::cout << "Error: " << log_filename << " could not be opened"
              << std::endl;
  }
  if (!random_file) {
    std::cout << "Error: " << random_filename << " not created" << std::endl;
  }

  int n_read = dist_int(gen);
  std::string line;
  while (getline(random_file, line)) {
    bool to_insert = dist_bool(gen);
    if (to_insert == false || isWhitespaceOrNewline(line)) {
      continue;
    }
    // std::cout << "Inserting line: " << line << std::endl;
    log_file << line << '\n';
  }

  log_file.close();
  random_file.close();
}

void updateTailLogfile(const std::string &tail_log_filename,
                       const std::string &log_filename, int n) {
  assert(n > 0);
  if (n == 0) {
    return;
  }
  FILE *log_file = fopen(log_filename.c_str(), "r");
  FILE *tail_log_file = fopen(tail_log_filename.c_str(), "w");

  fseek(log_file, 0, SEEK_END);
  long long logFileSizeBytes = ftell(log_file);
  long long logFileLines = 0;

  long long i = logFileSizeBytes - 1;
  for (; i >= 0; i--) {
    fseek(log_file, i, SEEK_SET);
    if (fgetc(log_file) != '\n') {
      continue;
    }
    logFileLines += 1;
    if (logFileLines == n + 1) {
      break;
    }
  }
  fseek(log_file, i + 1, SEEK_SET);

  char *line_buffer = (char *)malloc(sizeof(char) * 1024);
  if (line_buffer == NULL) {
    std::cout << "Error: failed to create line_buffer" << std::endl;
    fclose(log_file);
    fclose(tail_log_file);
    free(line_buffer);
    return;
  }
  while (fgets(line_buffer, sizeof(line_buffer), log_file)) {
    // std::cout << "Printing to tail_log line: " << line_buffer << std::endl;
    fprintf(tail_log_file, "%s", line_buffer);
  }
  fclose(log_file);
  fclose(tail_log_file);
  free(line_buffer);
}

int main() {
  // truncates existing log_file
  std::ofstream log_file(log_filename, std::ios::trunc);
  log_file.close();

  int i = 0;
  while (true) {
    // std::cout << "Iteration " << i << ":" << std::endl;
    appendToLogfile(log_filename, random_filename);
    updateTailLogfile(tail_log_filename, log_filename, TAIL_LINES);
    std::this_thread::sleep_for(std::chrono::seconds(dist_int(gen)));
  }
}