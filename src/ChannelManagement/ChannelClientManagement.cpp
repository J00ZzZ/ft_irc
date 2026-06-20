#include "../../inc/Channel.hpp"
#include "../../inc/utils.hpp"
#include "../../inc/Server.hpp"

void Channel::addClient(Client *c) {
	for (std::vector<Client *>::iterator it = this->m_clients.begin(); it != this->m_clients.end(); it++) {
		if (*it == c)
			return;
	}
	bool firstClient = m_clients.empty();
	this->m_clients.push_back(c);
	if (firstClient)
		this->addOperator(c);
}

void Channel::addClientToInvites(Client *c) {
	m_invitedClients.push_back(c);
}

void Channel::rmClientFromInvites(Client *c) {
	std::vector<Client*>::iterator it = std::find(m_invitedClients.begin(), m_invitedClients.end(), c);
	if (it != m_invitedClients.end())
		m_invitedClients.erase(it);
}

void	Channel::delClient(Client *c) {
	for (std::vector<Client *>::iterator it = this->m_clients.begin(); it != this->m_clients.end(); it++) {
		if (*it == c) {
			this->m_clients.erase(it);
			return;
		}
	}
}

void	Channel::addOperator(Client *c) {
	for (std::vector<Client *>::iterator it = this->m_operators.begin(); it != this->m_operators.end(); it++) {
		if (*it == c)
			return;
	}
	this->m_operators.push_back(c);

}

void Channel::delOperator(Client* c) {
	std::vector<Client*>::iterator it = std::find(m_operators.begin(), m_operators.end(), c);
	if (it != m_operators.end())
		m_operators.erase(it);
}
