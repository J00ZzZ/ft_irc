#include "../../inc/Server.hpp"
#include "../../inc/utils.hpp"
#include "../../inc/ServerReplies.hpp"

/**
 * Sends a response to all clients currently on a channel, excluding one if specified.
 *
 * @param ch The channel to send the response to.
 * @param response The string to send to all connected clients.
 * @param excluded_sender If provided, the client to exclude from the dispatch. (pass NULL to skip)
 */
void Server::dispatchToChannel(Channel *ch, std::string response, Client* excluded_sender) {
	std::vector<Client *> clients = ch->getClients();

	for (std::size_t i = 0; i < clients.size(); ++i) {
		if (clients[i] == NULL) {
			std::cerr << "[WARN] dispatchToChannel: NULL client pointer at index " << i << std::endl;
			continue;
		}
		if (clients[i] == excluded_sender)
			continue;
		int fd = clients[i]->getFd();
		int status = send(fd, response.c_str(), response.length(), 0);
		if (status == -1)
			std::cerr << "send() ERROR: failed to send reponse to socket_fd: " << fd << std::endl;
	}
}

/**
 * Sends a response to the specified socket_fd.
 *
 * @param response The string to send to the client.
 * @param fd The file descriptor of the socket to send the response to.
 */
void Server::sendResponse(std::string response, int fd) {
	int status = send(fd, response.c_str(), response.length(), 0);
	if (status == -1)
		std::cerr << "send() ERROR: failed to send reponse to socket_fd: " << fd << std::endl;
}

/**
 * Sends the NAMES reply to a client, listing all users in the specified channel.
 * 
 * Constructs and sends two messages: one containing the list of users and another
 * indicating the end of the NAMES list. If a user is an operator, their nickname
 * is prefixed with '@'.
 *
 * @param client The client to whom the NAMES reply is sent.
 * @param channel The channel from which the user list is retrieved.
 */
void Server::sendNameReply(Client* client, Channel* channel) {
	std::string userList;
	const std::vector<Client *>& clients = channel->getClients();

	for (std::size_t i = 0; i < clients.size(); ++i) {
		Client* c = clients[i];
		std::string prefix;

		if (channel->isOperator(c))
			prefix = "@";

		userList += prefix + c->getNick() + " ";
	}
	if (!userList.empty())
		userList.erase(userList.end() - 1);

	std::string r1 = RPL_NAMREPLY(getHostname(), client->getNick(), "=", channel->getName(), userList);
	std::string r2 = RPL_ENDOFNAMES(getHostname(), client->getNick(), channel->getName());

	sendResponse(r1, client->getFd());
	sendResponse(r2, client->getFd());
}
