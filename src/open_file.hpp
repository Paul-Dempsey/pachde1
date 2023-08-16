#pragma once
#ifndef OPEN_FILE_HPP_INCLUDED
#define OPEN_FILE_HPP_INCLUDED
#include <string>

namespace pachde {

bool openFileDialog(std::string folder, std::string filters, std::string filename, std::string& result);

}
#endif