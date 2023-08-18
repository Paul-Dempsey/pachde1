#pragma once
#ifndef OPEN_FILE_HPP_INCLUDED
#define OPEN_FILE_HPP_INCLUDED
#include <string>

namespace pachde {

bool openFileDialog(const std::string& folder, const std::string& filters, const std::string& filename, std::string& result);

}
#endif