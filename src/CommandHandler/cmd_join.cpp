#include "../../inc/CommandHandler.hpp"
#include "../../inc/Server.hpp"
#include "../../inc/ServerReplies.hpp"
#include "../../inc/utils.hpp"


/**
 * Parses a command to create a mapping of channel names to their respective keys.
 *
 * Extracts channel names from the command and maps them to keys if provided.
 * If keys are not provided or fewer keys than channels are provided, the remaining channels
 * are mapped to empty strings.
 * 
 * @return A map where keys are channel names and values are the corresponding keys.
 */
std::map<std::string, std::string> CommandHandler::_createChannelKeyMap(std::vector<std::string> & cmd) {
	std::map<std::string, std::string> channel_key_map;

	/* channel name extraction */
	std::vector<std::string> channel_names;
	std::string channel_str = cmd[1];
	std::size_t pos;

	while ((pos = channel_str.find(",")) != std::string::npos) {
		channel_names.push_back(channel_str.substr(0, pos));
		channel_str.erase(0, pos + 1);
	}

	if (!channel_str.empty())
		channel_names.push_back(channel_str);

	for (std::size_t i = 0; i < channel_names.size(); ++i) {
		channel_key_map[channel_names[i]] = "";
	}

	/* on keys provided: map the keys to channel names */
	if (cmd.size() >= 3) {
		std::vector<std::string> channel_keys;
		std::string keys_str = cmd[2];
		
		while ((pos = keys_str.find(",")) != std::string::npos) {
			channel_keys.push_back(keys_str.substr(0, pos));
			keys_str.erase(0, pos + 1);
		}
		if (!keys_str.empty())
			channel_keys.push_back(keys_str);

		for (std::size_t i = 0; i < channel_names.size(); ++i) {
			std::string key = (i < channel_keys.size()) ? channel_keys[i] : "";
			channel_key_map[channel_names[i]] = key;
		}
	}
	return (channel_key_map);
}

/**
 * Handles the JOIN command from a client, allowing them to join specified channels.
 *
 * Validates the command parameters and checks for channel restrictions:
 * invite-only status, ban status, channel key requirement, and capacity limit.
 * Sends appropriate error responses if these conditions are not met. If the channel does not exist,
 * it is created and the client is added as an operator. On successful joining, broadcasts
 * the join message to the channel and sends the channel topic and member list.
 *
 * @param fd The file descriptor representing the client connection.
 * @param cmd The command vector containing the channel names and optional keys.
 * 
 * @note Not all numeric replies work properly on halloy (our reference client), yet- they work very nice on other clients (e.g. irssi, weechat)
 */
void CommandHandler::_handleJoinCmd(int fd, std::vector<std::string> & cmd) {
	if (cmd.size() < 2) {
		m_Server.sendResponse(ERR_NEEDMOREPARAMS(m_Server.getHostname(), m_Server.getClientByFd(fd)->getNick(), "JOIN"), fd);
		return;
	}
	
	Client* client = m_Server.getClientByFd(fd);
	std::map<std::string, std::string> channel_key_map = _createChannelKeyMap(cmd);

	for (std::map<std::string, std::string>::iterator it = channel_key_map.begin(); it != channel_key_map.end(); ++it) {
		std::string ch_name = it->first;
		std::string key = it->second;
		char* time_str = get_time_string();

		std::cout << PURPLE << "[" << time_str << "]" << RESET
		          << " JOIN: Client " << client->getNick() << " attempts to join " << ch_name << std::endl;

		if (!Channel::isValidName(ch_name)) {
			std::cout << PURPLE << "[" << time_str << "]" << RESET
			          << " JOIN: Invalid channel name: " << ch_name << std::endl;
			m_Server.sendResponse(ERR_BADCHANMASK(m_Server.getHostname(), client->getNick(), ch_name), fd);
			continue;
		}

		Channel* channel = m_Server.getChannel(ch_name);
		if (!channel) {
			std::cout << PURPLE << "[" << time_str << "]" << RESET
			          << " JOIN: Creating new channel: " << ch_name << std::endl;
			channel = new Channel(&m_Server, ch_name);
			channel->addOperator(client);
			m_Server.addChannel(channel);
		}

		if (channel->isInviteOnly() && !channel->isInvited(client)) {
			std::cout << PURPLE << "[" << time_str << "]" << RESET
			          << " JOIN: Rejected — invite-only channel." << std::endl;
			m_Server.sendResponse(ERR_INVITEONLYCHAN(m_Server.getHostname(), client->getNick(), channel->getName()), fd);
			continue;
		}

		if (channel->isBanned(client)) {
			std::cout << PURPLE << "[" << time_str << "]" << RESET
			          << " JOIN: Rejected — client is banned." << std::endl;
			m_Server.sendResponse(ERR_BANNEDFROMCHAN(m_Server.getHostname(), client->getNick(), channel->getName()), fd);
			continue;
		}
		
		if (!channel->checkKey(key)) {
			std::cout << PURPLE << "[" << time_str << "]" << RESET
			          << " JOIN: Rejected — incorrect key." << std::endl;
			m_Server.sendResponse(ERR_BADCHANNELKEY(m_Server.getHostname(), client->getNick(), ch_name), fd);
			continue;
		}

		if (channel->isChannelFull()) {
			std::cout << PURPLE << "[" << time_str << "]" << RESET
			          << " JOIN: Rejected — channel is full." << std::endl;
			m_Server.sendResponse(ERR_CHANNELISFULL(m_Server.getHostname(), client->getNick(), ch_name), fd);
			continue;
		}

		std::cout << PURPLE << "[" << time_str << "]" << RESET
		          << " JOIN: " << client->getNick() << " joined " << ch_name << std::endl;

		channel->addClient(client);
		client->addChannel(channel);

		m_Server.dispatchToChannel(channel, ":" + client->getFullId() + " JOIN " + ch_name + "\r\n", NULL);
		channel->forwardChanCache(fd);

		if (channel->getTopic() != "") {
			std::cout << PURPLE << "[" << time_str << "]" << RESET
			          << " JOIN: Sending topic for " << ch_name << std::endl;
			m_Server.sendResponse(RPL_TOPIC(m_Server.getHostname(), client->getNick(), ch_name, channel->getTopic()), fd);
		} else {
			std::cout << PURPLE << "[" << time_str << "]" << RESET
			          << " JOIN: Sending no topic reply for " << ch_name << std::endl;
			m_Server.sendResponse(RPL_NOTOPIC(m_Server.getHostname(), client->getNick(), ch_name), fd);
		}

		std::cout << PURPLE << "[" << time_str << "]" << RESET
		          << " JOIN: Sending name list for " << ch_name << std::endl;
		m_Server.sendNameReply(client, channel);
	}
}

