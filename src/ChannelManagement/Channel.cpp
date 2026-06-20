#include "../../inc/Channel.hpp"
#include "../../inc/utils.hpp"
#include "../../inc/Server.hpp"

Channel::Channel(Server *serv, const std::string &title) : m_server(serv), m_clients(), m_name(title), m_topic(""),
	m_key(""), m_memberLimit(50), m_inviteOnly(false), m_topicOperatorPrivilege(false) {
	m_cache_filename = generate_cache_name(title);
}

Channel::Channel(const Channel &other) : m_server(other.m_server), m_clients(other.m_clients), m_name(other.m_name) { }

Channel::~Channel(void) {
	if (!m_cache_filename.empty()) {
		std::remove(m_cache_filename.c_str());
	}
}

Channel	&Channel::operator = (const Channel &other) {
	this->m_server = other.m_server;
	this->m_clients = other.m_clients;
	this->m_name = other.m_name;

	return (*this);
}
