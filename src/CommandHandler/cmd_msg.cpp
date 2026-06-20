#include "../../inc/CommandHandler.hpp"
#include "../../inc/Server.hpp"
#include "../../inc/ServerReplies.hpp"
#include "../../inc/utils.hpp"

/**
 * Handles the MSG / PRIVMSG command, allowing a user to send a private message to one or more recipients.
 *
 * Protocol checks:
 * - Ensures the command has sufficient parameters and a valid recipient.
 * - Extracts and validates the recipient names and message content.
 * - Sends appropriate error responses if conditions are not met.
 * - If the recipient is a channel, verifies its existence and dispatches the message to channel members.
 * - If the recipient is a user, verifies their existence and sends the message directly.
 *
 * @param fd The file descriptor representing the client connection.
 * @param cmd The command vector containing the recipient list and message content.
 */
void CommandHandler::_handleMsgCmd(int fd, std::vector<std::string>& cmd) {
	Client* sender = m_Server.getClientByFd(fd);
	char* time_str = get_time_string();

	if (cmd.size() < 3 || cmd[1].empty()) {
		std::cout << CYAN << "[" << time_str << "]" << RESET
		          << " PRIVMSG: Rejected — no recipient" << std::endl;
		m_Server.sendResponse(ERR_NORECIPIENT(m_Server.getHostname(), sender->getNick(), "PRIVMSG"), fd);
		return;
	}

	/* receiver name extraction */
	std::vector<std::string> receivers;
	std::string receiver_str = cmd[1];
	std::size_t pos;

	while ((pos = receiver_str.find(",")) != std::string::npos) {
		receivers.push_back(receiver_str.substr(0, pos));
		receiver_str.erase(0, pos + 1);
	}
	if (!receiver_str.empty())
		receivers.push_back(receiver_str);

	/* extract message */
	std::string msg;
	for (std::size_t i = 2; i < cmd.size(); ++i)
		msg += (i == 2 ? "" : " ") + cmd[i];

	if (msg.empty()) {
		std::cout << CYAN << "[" << time_str << "]" << RESET
		          << " PRIVMSG: Rejected — no text to send" << std::endl;
		m_Server.sendResponse(ERR_NOTEXTTOSEND(m_Server.getHostname(), sender->getNick()), fd);
		return;
	}

	for (std::size_t i = 0; i < receivers.size(); ++i) {
		std::string& target = receivers[i];
		std::string fullMsg = ":" + sender->getFullId() + " PRIVMSG " + target + " :" + msg + "\r\n";

		if (fullMsg.length() > IRC_MESSAGE_BUFFERSIZE) {
			m_Server.sendResponse(ERR_MSGTOOLONG(m_Server.getHostname(), sender->getNick()), fd);
			return ;
		}
		
		if (m_Server.getBot()->notifyOnCensorship(msg, fd)) {
			return ;
		}

		if (target[0] == '#') {
			Channel* channel = m_Server.getChannel(target);
			if (!channel) {
				std::cout << CYAN << "[" << time_str << "]" << RESET
				          << " PRIVMSG: Rejected — no such channel: " << target << std::endl;
				m_Server.sendResponse(ERR_NOSUCHCHANNEL(m_Server.getHostname(), sender->getNick(), target), fd);
			} else {
				std::cout << CYAN << "[" << time_str << "]" << RESET
				          << " PRIVMSG: " << sender->getNick()
				          << " -> channel " << target
				          << " | msg: " << msg << std::endl;
				if (!channel->isClientOnChannel(sender)) {
					m_Server.sendResponse(ERR_NOTONCHANNEL(m_Server.getHostname(), sender->getNick(), target), fd);
					return ;
				}
				m_Server.dispatchToChannel(channel, fullMsg, sender);
				channel->writeToCache(fullMsg);
			}
		} else {
			Client* receiver = m_Server.getClientByNick(target);
			if (!receiver) {
				std::cout << CYAN << "[" << time_str << "]" << RESET
				          << " PRIVMSG: Rejected — no such nick: " << target << std::endl;
				m_Server.sendResponse(ERR_NOSUCHNICK(m_Server.getHostname(), sender->getNick(), target), fd);
			} else {
				std::cout << CYAN << "[" << time_str << "]" << RESET
				          << " PRIVMSG: " << sender->getNick()
				          << " -> user " << receiver->getNick()
				          << " | msg: " << msg << std::endl;
				m_Server.sendResponse(fullMsg, receiver->getFd());
			}
		}
	}
}

