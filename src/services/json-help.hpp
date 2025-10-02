#pragma once
#include <rack.hpp>

std::string get_json_string(const json_t* root, const char* key);
std::string get_json_string(const json_t* root, const char* key, const std::string& default_value);
bool get_json_bool(const json_t* root, const char* key, bool default_value);
float get_json_float(const json_t* root, const char* key, float default_value);
int get_json_int(const json_t* root, const char* key, int default_value);
int64_t get_json_int64(const json_t* root, const char* key, int64_t default_value);

inline void set_json(json_t* root, const char* key, const std::string& value) {
    json_object_set_new(root, key, json_stringn(value.c_str(), value.size()));
}

inline void set_json_int(json_t* root, const char* key, int value) {
    json_object_set_new(root, key, json_integer(value));
}

inline void set_json(json_t* root, const char* key, bool value) {
    json_object_set_new(root, key, json_boolean(value));
}

inline void set_json(json_t* root, const char* key, float value) {
    json_object_set_new(root, key, json_real(value));
}