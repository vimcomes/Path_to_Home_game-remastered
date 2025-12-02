#pragma once

#include <string>
#include <string_view>

std::string resourcePath(std::string_view relative);
std::string executableRelative(std::string_view relative);
