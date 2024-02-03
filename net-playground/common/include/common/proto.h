#pragma once

#include <memory>
#include <vector>
#include <string>

std::shared_ptr<std::vector<uint8_t>> create_message(const std::string& content);
