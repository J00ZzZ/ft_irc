#include "../../inc/Bot.hpp"
#include "../../inc/ServerReplies.hpp"
#include "../../inc/Server.hpp"

Bot::Bot(Server &server) : _server(server), _name("Claptrap") {
    _loadBannedWords();
}

Bot::~Bot(void) {}


/**
 * Reads a file containing banned words and stores them in _bannedWords.
 * Each word must be separated by a whitespace.
 * If the file can't be opened, an error message is printed to stderr.
 */
void Bot::_loadBannedWords(void) {
    std::ifstream file("config/banned.txt");
    if (!file.is_open()) {
        std::cerr << "ERROR: failed to open banned words file" << std::endl;
        return;
    }
    std::string word;
    while (file >> word)
        _bannedWords.push_back(word);
    file.close();
}

/**
 * Checks if a message contains a banned word. If so, sends a response to the client
 * at socket_fd, warning them about the use of banned words.
 *
 * @param msg The message to check
 * @param fd The socket FD to send the response to
 * @return 1 if a banned word was found, 0 otherwise
 */
int Bot::notifyOnCensorship(const std::string& msg, int fd) {
    std::vector<std::string>::const_iterator it;
    for (it = _bannedWords.begin(); it != _bannedWords.end(); ++it) {
        if (msg.find(*it) != std::string::npos) {
            _server.sendResponse(RPL_CENSORSHIP(_name, _server.getClientByFd(fd)->getNick(), *it), fd);
            return (1);
        }
    }
    return (0);
}
