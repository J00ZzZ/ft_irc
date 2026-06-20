#include "../../inc/CommandHandler.hpp"
#include "../../inc/Server.hpp"
#include "../../inc/ServerReplies.hpp"
#include "../../inc/utils.hpp"

/**
 * Handles the INVITE command, allowing an operator to invite another user to a channel.
 * 
 *  Protocol checks:
 * - Parameter count
 * - Target user and channel validity
 * - Operator privilege and membership checks
 * - Duplicate invitation prevention
 * 
 * On success, notifies the invitor and sends an INVITE command to the target.
 * 
 * @param fd The file descriptor representing the client connection.
 * @param cmd The command vector containing the target nickname and channel name.
 */
void CommandHandler::_handleInviteCmd(int fd, std::vector<std::string>& cmd) {
    char* time_str = get_time_string();

    if (cmd.size() != 3) {
        std::cout << PURPLE << "[" << time_str << "]" << RESET
                  << " INVITE: Missing parameters" << std::endl;
        m_Server.sendResponse(ERR_NEEDMOREPARAMS(m_Server.getHostname(), m_Server.getClientByFd(fd)->getNick(), "INVITE"), fd);
        return ;
    }

    Client* invitor = m_Server.getClientByFd(fd);
    Client* target = m_Server.getClientByNick(cmd[1]);

    std::cout << PURPLE << "[" << time_str << "]" << RESET
              << " INVITE: " << invitor->getNick() << " attempts to invite " << cmd[1] << " to " << cmd[2] << std::endl;

    if (!target) {
        std::cout << PURPLE << "[" << time_str << "]" << RESET
                  << " INVITE: Target user " << cmd[1] << " does not exist" << std::endl;
        m_Server.sendResponse(ERR_NOSUCHNICK(m_Server.getHostname(), invitor->getNick(), cmd[1]), fd);
        return ;
    }

    Channel* channel = m_Server.getChannel(cmd[2]);
    if (!channel) {
        std::cout << PURPLE << "[" << time_str << "]" << RESET
                  << " INVITE: Channel " << cmd[2] << " does not exist" << std::endl;
        m_Server.sendResponse(ERR_NOSUCHCHANNEL(m_Server.getHostname(), invitor->getNick(), cmd[2]), fd);
        return ;
    }

    if (!channel->isOperator(invitor)) {
        std::cout << PURPLE << "[" << time_str << "]" << RESET
                  << " INVITE: " << invitor->getNick() << " is not a channel operator" << std::endl;
        m_Server.sendResponse(ERR_CHANOPRIVSNEEDED(m_Server.getHostname(), invitor->getNick(), channel->getName()), fd);
        return ;
    }

    if (!channel->isClientOnChannel(invitor)) {
        std::cout << PURPLE << "[" << time_str << "]" << RESET
                  << " INVITE: " << invitor->getNick() << " is not on the channel" << std::endl;
        m_Server.sendResponse(ERR_NOTONCHANNEL(m_Server.getHostname(), invitor->getNick(), channel->getName()), fd);
        return ;
    }

    if (channel->isClientOnChannel(target)) {
        std::cout << PURPLE << "[" << time_str << "]" << RESET
                  << " INVITE: Target " << target->getNick() << " is already on " << channel->getName() << std::endl;
        m_Server.sendResponse(ERR_USERONCHANNEL(m_Server.getHostname(), invitor->getNick(), target->getNick(), channel->getName()), fd);
        return ;
    }

    std::cout << PURPLE << "[" << time_str << "]" << RESET
              << " INVITE: " << target->getNick() << " has been invited to " << channel->getName() << std::endl;

    m_Server.sendResponse(RPL_INVITING(m_Server.getHostname(), invitor->getNick(), target->getNick(), channel->getName()), fd);
    m_Server.sendResponse(CMD_INVITE(invitor->getNick(), target->getNick(), channel->getName()), target->getFd());
    
    channel->addClientToInvites(target);
}
