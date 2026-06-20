#include "../../inc/CommandHandler.hpp"
#include "../../inc/Server.hpp"
#include "../../inc/ServerReplies.hpp"
#include "../../inc/utils.hpp"

void CommandHandler::_toggleI_mode(Channel* channel, Client* client, std::string& option) {
    if (option == "+i") {
        if (!channel->isInviteOnly()) {
            channel->toggleInviteOnly();
            m_Server.dispatchToChannel(channel, RPL_MODE_CHANGE(client->getNick(), channel->getName(), "+i"), NULL);
        }
    } else if (option == "-i") {
        if (channel->isInviteOnly()) {
            channel->toggleInviteOnly();
            m_Server.dispatchToChannel(channel, RPL_MODE_CHANGE(client->getNick(), channel->getName(), "-i"), NULL);
        }
    }
}

void CommandHandler::_toggleT_mode(Channel *channel, Client* client, std::string& option) {
    if (option == "+t") {
        if (!channel->getTopicPrivilege()) {
            channel->toggleTopicPrivilege();
            m_Server.dispatchToChannel(channel, RPL_MODE_CHANGE(client->getNick(), channel->getName(), "+t"), NULL);
        }
    } else if (option == "-t") {
        if (channel->getTopicPrivilege()) {
            channel->toggleTopicPrivilege();
            m_Server.dispatchToChannel(channel, RPL_MODE_CHANGE(client->getNick(), channel->getName(), "-t"), NULL);
        }
    }
}

void CommandHandler::_toggleK_mode(Channel* channel, Client* client, std::vector<std::string>& cmd) {
    std::string option = cmd[2];
    if (option == "-k") {
        if (channel->getKey() != "") {
            channel->setKey("");
            m_Server.dispatchToChannel(channel, RPL_MODE_CHANGE(client->getNick(), channel->getName(), "-k"), NULL);
        }
    } else if (option == "+k") {
        if (cmd.size() != 4) {
            m_Server.sendResponse(ERR_NEEDMOREPARAMS(m_Server.getHostname(), client->getNick(), "+k"), client->getFd());
            return ;
        }
        channel->setKey(cmd[3]);
        m_Server.dispatchToChannel(channel, RPL_MODE_CHANGE(client->getNick(), channel->getName(), "+k"), NULL);
    }
}

void CommandHandler::_toggleO_mode(Channel* channel, Client* client, std::vector<std::string>& cmd) {
    if (cmd.size() < 4) {
        m_Server.sendResponse(ERR_NEEDMOREPARAMS(m_Server.getHostname(), client->getNick(), "MODE"), client->getFd());
        return;
    }

    if (!channel->isOperator(client)) {
        m_Server.sendResponse(ERR_CHANOPRIVSNEEDED(m_Server.getHostname(), client->getNick(), channel->getName()), client->getFd());
        return;
    }

    Client* target = m_Server.getClientByNick(cmd[3]);
    if (!target) {
        m_Server.sendResponse(ERR_NOSUCHNICK(m_Server.getHostname(), client->getNick(), cmd[3]), client->getFd());
        return;
    }

    std::string option = cmd[2];

    if (option == "+o") {
        if (!channel->isOperator(target)) {
            channel->addOperator(target);
            m_Server.dispatchToChannel(channel, RPL_MODE_CHANGE(client->getFullId(), channel->getName(), "+o " + target->getNick()), NULL);
        }
    } else if (option == "-o") {
        if (channel->isOperator(target)) {
            channel->delOperator(target);
            m_Server.dispatchToChannel(channel, RPL_MODE_CHANGE(client->getFullId(), channel->getName(), "-o " + target->getNick()), NULL);
        }
    }
}
	
void	CommandHandler::_toggleL_mode(Channel *channel, Client *client, std::vector<std::string> &cmd) {
    if (cmd.size() < 3) {
        m_Server.sendResponse(ERR_NEEDMOREPARAMS(m_Server.getHostname(), client->getNick(), "MODE"), client->getFd());
        return;
    }

    if (!channel->isOperator(client)) {
        m_Server.sendResponse(ERR_CHANOPRIVSNEEDED(m_Server.getHostname(), client->getNick(), channel->getName()), client->getFd());
        return;
    }

    Channel* target = m_Server.getChannel(cmd[1]);
    if (!target) {
        m_Server.sendResponse(ERR_NOSUCHNICK(m_Server.getHostname(), client->getNick(), cmd[1]), client->getFd());
        return;
    }

    std::string option = cmd[2];

    if (option == "+l") {
		if (cmd.size() < 4) {
			m_Server.sendResponse(ERR_NEEDMOREPARAMS(m_Server.getHostname(), client->getNick(), "MODE"), client->getFd());
			return;
		}
        if (channel->isOperator(client)) {
			target->setMemberLimit(std::atoi(cmd.at(3).c_str()));
            m_Server.dispatchToChannel(channel, RPL_MODE_CHANGE(client->getFullId(), channel->getName(), "+l " + client->getNick()), NULL);
        }
    } else if (option == "-l") {
        if (channel->isOperator(client)) {
			target->setMemberLimit(50);
            m_Server.dispatchToChannel(channel, RPL_MODE_CHANGE(client->getFullId(), channel->getName(), "-l " + client->getNick()), NULL);
        }
    }
}

void CommandHandler::_handleModeCmd(int fd, std::vector<std::string>& cmd) {
    if (cmd.size() < 2) {
        m_Server.sendResponse(ERR_NEEDMOREPARAMS(m_Server.getHostname(), m_Server.getClientByFd(fd)->getNick(), "MODE"), fd);
        return ;
    }
    
    Client *client = m_Server.getClientByFd(fd);
    Channel *channel = m_Server.getChannel(cmd[1]);
    /* second condition is a ducktape: irssi sends user mode query command on connection which we dont handle.
        It prevents the ghost "no such channel" reply */
    if (!channel) {
        if (client->getNick() != cmd[1])
            m_Server.sendResponse(ERR_NOSUCHCHANNEL(m_Server.getHostname(), m_Server.getClientByFd(fd)->getNick(), cmd[1]), fd);
        return ;
    }

    if (!channel->isOperator(client)) {
        m_Server.sendResponse(ERR_CHANOPRIVSNEEDED(m_Server.getHostname(), client->getNick(), channel->getName()), fd);
        return ;
    }

    if (cmd.size() == 2 && channel->getName() == cmd[1]) // -> TODO: mode query
        return ;

    if (cmd[2] == "+i" || cmd[2] == "-i") {
        _toggleI_mode(channel, client, cmd[2]);
    } else if (cmd[2] == "+t" || cmd[2] == "-t") {
        _toggleT_mode(channel, client, cmd[2]);
    } else if (cmd[2] == "+k" || cmd[2] == "-k") {
        _toggleK_mode(channel, client, cmd);
    } else if (cmd[2] == "+o" || cmd[2] == "-o") {
        _toggleO_mode(channel, client, cmd);
    } else if (cmd[2] == "+l" || cmd[2] == "-l") {
        _toggleL_mode(channel, client, cmd);
    } else {
        m_Server.sendResponse(ERR_UNKNOWNMODE(m_Server.getHostname(), m_Server.getClientByFd(fd)->getNick(), cmd[2]), fd);
    }
}
