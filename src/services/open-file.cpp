#include <rack.hpp>
#include <osdialog.h>
#include "open-file.hpp"

using namespace ::rack;

namespace pachde {

bool fileDialog(osdialog_file_action action, const std::string& folder, const std::string& filters, const std::string& filename, std::string& result)
{
    osdialog_filters* osd_filters = osdialog_filters_parse(filters.c_str());
    DEFER({osdialog_filters_free(osd_filters);});

    std::string osd_dir = folder.empty() ? asset::user("") : folder;
    std::string osd_name = system::getFilename(filename);
    char* cpath = osdialog_file(action, osd_dir.c_str(), osd_name.c_str(), osd_filters);
    if (!cpath) {
        result.clear();
        return false;
    } else {
        result = cpath;
        std::free(cpath);
        return true;
    }
}

bool openFileDialog(const std::string& folder, const std::string& filters, const std::string& filename, std::string& result)
{
    return fileDialog(OSDIALOG_OPEN, folder, filters, filename, result);
}

bool saveFileDialog(const std::string& folder, const std::string& filters, const std::string& filename, std::string& result)
{
    return fileDialog(OSDIALOG_SAVE, folder, filters, filename, result);
}


}