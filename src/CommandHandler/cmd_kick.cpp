#include "../../inc/CommandHandler.hpp"
#include "../../inc/Server.hpp"
#include "../../inc/ServerReplies.hpp"
#include "../../inc/utils.hpp"

/**
 * Handles the KICK command, allowing a user to kick another user from a channel.
 *
 * Protocol checks:
 * - Parameter count
 * - Target channel and user validity
 * - Operator privilege and membership checks
 *
 * On success, performs the kick and sends the command to the target and channel.
 *
 * @param fd The file descriptor representing the client connection.
 * @param cmd The command vector containing the channel name and target nickname.
 */
void CommandHandler::_handleKickCmd(int fd, std::vector<std::string>& cmd) {
	std::string time_str = get_time_string();
	Client* sender = m_Server.getClientByFd(fd);
	if (!sender) return;

	if (cmd.size() < 3) {
		std::cout << BLUE << "[" << time_str << "]" << RESET
		          << " KICK: Missing parameters" << std::endl;
		m_Server.sendResponse(ERR_NEEDMOREPARAMS(m_Server.getHostname(), sender->getNick(), "KICK"), fd);
		return;
	}

	std::vector<std::string> channels;
	std::vector<std::string> users;

	std::string channel_arg = cmd[1];
	std::size_t pos;
	while ((pos = channel_arg.find(',')) != std::string::npos) {
		channels.push_back(channel_arg.substr(0, pos));
		channel_arg.erase(0, pos + 1);
	}
	channels.push_back(channel_arg);

	// usr list
	std::string user_arg = cmd[2];
	while ((pos = user_arg.find(',')) != std::string::npos) {
		users.push_back(user_arg.substr(0, pos));
		user_arg.erase(0, pos + 1);
	}
	users.push_back(user_arg);

	std::cout << BLUE << "[" << time_str << "]" << RESET
	          << " KICK: Parsed " << channels.size() << " channel(s), "
	          << users.size() << " user(s)" << std::endl;

	if (channels.size() != users.size()) {
		std::cout << BLUE << "[" << time_str << "]" << RESET
		          << " KICK: Channel/user count mismatch" << std::endl;
		m_Server.sendResponse(ERR_NEEDMOREPARAMS(m_Server.getHostname(), sender->getNick(), "KICK"), fd);
		return;
	}

	// exec each kick
	for (std::size_t i = 0; i < channels.size(); ++i) {
		std::string& chan_name = channels[i];
		std::string& target_nick = users[i];

		Channel* chan = m_Server.getChannel(chan_name);
		Client* target = m_Server.getClientByNick(target_nick);

		if (!chan) {
			m_Server.sendResponse(ERR_NOSUCHCHANNEL(m_Server.getHostname(), sender->getNick(), chan_name), fd);
			continue;
		}
		if (!target || !chan->isClientOnChannel(target)) {
			m_Server.sendResponse(ERR_USERNOTINCHANNEL(m_Server.getHostname(), target_nick, chan_name), fd);
			continue;
		}
		if (!chan->isOperator(sender)) {
			m_Server.sendResponse(ERR_CHANOPRIVSNEEDED(m_Server.getHostname(), sender->getNick(), chan_name), fd);
			continue;
		}

		// Perform the kick
		chan->delOperator(target);
		chan->rmClientFromInvites(target);
		chan->delClient(target);

		std::string kick_msg = "because";
		if (cmd.size() > 3) {
			std::string reason;
			for (size_t j = 3; j < cmd.size(); ++j) {
				if (!reason.empty()) reason += " ";
				reason += cmd[j];
			}
			kick_msg = reason;
		}

		std::string full_id = sender->getFullId();
		std::string reply = RPL_KICK(full_id, chan_name, target_nick, kick_msg);

		m_Server.dispatchToChannel(chan, reply, NULL);
		m_Server.sendResponse(reply, target->getFd());
	}
}

