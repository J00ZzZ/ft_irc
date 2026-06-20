#include "../../inc/Server.hpp"
#include "../../inc/utils.hpp"
#include "../../inc/ServerReplies.hpp"

/**
 * Handles the PASS command sent by a client.
 *
 * Sends an error message back to the client on invalid number of parameters.
 * Then it checks if the password sent by the client matches the server's password. If it does
 * not, it sends an error message back to the client.
 */
void Server::handlePasswordAuth(int processed_fd, std::vector<std::string> cmd_vec) {
	Client* processed_client = getClientByFd(processed_fd);
	if (!processed_client)
		return;

	std::string nick = processed_client->getNick();
	if (nick.empty())
		nick = "*";

	if (cmd_vec.size() != 2) {
		sendResponse(ERR_NEEDMOREPARAMS(_hostname, nick, "PASS"), processed_fd);
		return;
	}

	if (_password != cmd_vec[1]) {
		sendResponse(ERR_PASSWDMISMATCH(_hostname, nick), processed_fd);
		return;
	}
	processed_client->setPassReceived();	
}

/**
 * Attempts to register a client with the server.
 *
 * Tries to register the client at the given fd. The registration is successful if
 * the client has already sent a valid NICK, USER and PASS command. If the registration
 * is successful, the client is sent a welcome message and marked as authenticated.
 */
void Server::tryRegister(int fd) {
	Client* client = getClientByFd(fd);
	if (!client)
		return;

	if (client->getNickReceived() && client->getPassReceived() && client->getUnameReceived()) {
		client->setAuth();
		sendResponse(RPL_WELCOME(_hostname, client->getNick(), client->getUsername(), client->getHostname()), fd);
	}
}