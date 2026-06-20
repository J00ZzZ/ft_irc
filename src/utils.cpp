#include "../inc/utils.hpp"

/**
 * Return a string with the current time, formatted as "%Y-%m-%d %H:%M:%S".
 * Used for logging in the server.
 * @return a pointer to the statically allocated string.
 */
char* get_time_string(void) {
    static char time_str[64];
    std::time_t now = std::time(NULL);
    std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return (time_str);
}

std::string sanitize_filename(const std::string& name) {
    std::string safe = name;
    for (size_t i = 0; i < safe.size(); ++i) {
        if (safe[i] == '#' || safe[i] == '/' || safe[i] == '\\' || safe[i] == ' ') {
            safe[i] = '_';
        }
    }
    return (safe);
}

std::string generate_cache_name(const std::string& name) {
    time_t t = time(NULL);
    struct tm* now = localtime(&t);

    char dateBuf[16];
    strftime(dateBuf, sizeof(dateBuf), "%Y%m%d", now);

    std::string safeName = sanitize_filename(name);

    std::ostringstream oss;
    oss << "cache/cache_" << safeName << "_" << dateBuf << ".log";

    return (oss.str());
}