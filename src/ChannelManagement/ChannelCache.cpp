#include "../../inc/Channel.hpp"
#include "../../inc/utils.hpp"
#include "../../inc/Server.hpp"

/**
 * Appends the given string to the cache file of this channel.
 *
 * @param cached The line to cache.
 */
void Channel::writeToCache(const std::string &cached) const {
	std::ofstream cache_file(m_cache_filename.c_str(), std::ios::app);
	if (cache_file) {
		cache_file << cached;
	} else {
		std::cerr << "ERROR: failed to cache on channel: " << m_name << std::endl;
	}
}

/**
 * Forwards all cached lines from this channel to a client.
 *
 * This sends all lines cached in the cache file for this channel to the
 * specified file descriptor. This is used to send cached messages to newly
 * joined clients.
 *
 * @param fd The file descriptor to send the cached messages to.
 */
void Channel::forwardChanCache(const int fd) const {
	std::ifstream cache(m_cache_filename.c_str());
	if (!cache.is_open()) {
		std::cerr << "ERROR: failed to open cache file for channel: " << m_name << std::endl;
		return;
	}

	std::string line;
	while (std::getline(cache, line)) {
		m_server->sendResponse(line + "\r\n", fd);
	}
	cache.close();
}
