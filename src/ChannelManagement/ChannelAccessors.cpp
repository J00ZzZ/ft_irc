#include "../../inc/Channel.hpp"
#include "../../inc/utils.hpp"
#include "../../inc/Server.hpp"

void Channel::toggleInviteOnly(void) {
	if (m_inviteOnly == true)
		m_inviteOnly = false;
	else
		m_inviteOnly = true;
}

void Channel::toggleTopicPrivilege(void) {
	if (m_topicOperatorPrivilege == true)
		m_topicOperatorPrivilege = false;
	else
		m_topicOperatorPrivilege = true;
}

void Channel::setTopic(std::string topic) {
	m_topic = topic;
}

void Channel::setKey(std::string key) {
	m_key = key;
}

void	Channel::setMemberLimit(std::size_t limit) {
	this->m_memberLimit = limit;
}

std::size_t		Channel::getMemberLimit(void) {
	return (this->m_memberLimit);
}

const std::string&	Channel::getName() const {
	return (this->m_name);
}

const std::vector<Client *>& Channel::getClients(void) const {
	return (this->m_clients);
}

const std::string& Channel::getTopic(void) const {
	return (this->m_topic);
}

const std::string& Channel::getCacheFilename(void) const {
	return (m_cache_filename);
}

const std::string& Channel::getKey(void) const {
	return (m_key);
}

bool Channel::getTopicPrivilege(void) const {
	return (m_topicOperatorPrivilege);
}

bool Channel::isValidName(std::string& name) {
	if (name.empty())
		return (false);
	if (name[0] != '#' && name[0] != '&')
		return (false);
	for (std::size_t i = 0; i < name.size(); ++i) {
		if (name[i] == ' ' || name[i] == ',' || name[i] == '\x07')
			return (false);
	}
	if (name.size() > 50)
		return (false);
	return (true);
}

bool Channel::isInviteOnly(void) const {
	return (this->m_inviteOnly);
}

bool Channel::isInvited(Client * c) const {
	for (std::size_t i = 0; i < this->m_invitedClients.size(); ++i) {
		if (this->m_invitedClients[i] == c)
			return (true);
	}
	return (false);
}

bool Channel::isBanned(Client * c) const {
	for (std::size_t i = 0; i < this->m_bannedClients.size(); ++i) {
		if (this->m_bannedClients[i] == c)
			return (true);
	}
	return (false);
}

bool Channel::isChannelFull(void) const {
	if (m_clients.size() >= m_memberLimit)
		return (true);
	return (false);
}

bool Channel::checkKey(std::string& key) const {
	if (key != this->m_key)
		return (false);
	return (true);
}

bool Channel::isOperator(Client *c) const {
	for (std::size_t i = 0; i < m_operators.size(); i++) {
		if (m_operators[i] == c)
			return (true);
	}
	return (false);
}

bool Channel::isClientOnChannel(Client *c) const {
	for (std::size_t i = 0; i < m_clients.size(); ++i) {
		if (m_clients[i] == c)
			return (true);
	}
	return (false);
}