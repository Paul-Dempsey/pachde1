#include "json-help.hpp"

std::string get_json_string(const json_t* root, const char* key, const std::string& default_value)
{
    auto j = json_object_get(root, key);
    if (!j) return default_value;
    auto s = json_string_value(j);
    if (!s) return default_value;
    return s;
}

std::string get_json_string(const json_t* root, const char* key)
{
    auto j = json_object_get(root, key);
    if (!j) return "";
    auto s = json_string_value(j);
    if (!s) return "";
    return s;
}

bool get_json_bool(const json_t* root, const char* key, bool default_value) {
    auto j = json_object_get(root, key);
    return j ? json_is_true(j) : default_value;
}

float get_json_float(const json_t* root, const char* key, float default_value) {
    auto j = json_object_get(root, key);
    if (j) {
        if (json_is_real(j)) return json_real_value(j);
        if (json_is_integer(j)) return json_integer_value(j);
    }
    return default_value;
}

int get_json_int(const json_t* root, const char* key, int default_value) {
    auto j = json_object_get(root, key);
    return j ? json_integer_value(j) : default_value;
}

int64_t get_json_int64(const json_t* root, const char* key, int64_t default_value) {
    auto j = json_object_get(root, key);
    return j ? json_integer_value(j) : default_value;
}


