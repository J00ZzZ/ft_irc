#include "../../inc/CommandHandler.hpp"
#include "../../inc/Server.hpp"
#include "../../inc/ServerReplies.hpp"
#include "../../inc/utils.hpp"

/**
 * Handles the TOPIC command, allowing a user to view or set the topic of a channel.
 *
 *  Protocol checks:
 * - Parameter count
 * - Target channel validity
 * - Topic privilege and operator checks
 *
 * On success, sends the topic to the client or sets the topic of the channel.
 *
 * @param fd The file descriptor representing the client connection.
 * @param cmd The command vector containing the channel name and optional new topic.
 */
void CommandHandler::_handleTopicCmd(int fd, std::vector<std::string>& cmd) {
    char* time_str = get_time_string();

    if (cmd.size() < 2) {
        std::cout << BLUE << "[" << time_str << "]" << RESET
                  << " TOPIC: Missing parameters" << std::endl;
        m_Server.sendResponse(ERR_NEEDMOREPARAMS(m_Server.getHostname(), m_Server.getClientByFd(fd)->getNick(), "TOPIC"), fd);
        return ;
    }

    Channel *channel = m_Server.getChannel(cmd[1]);
    if (!channel) {
        std::cout << BLUE << "[" << time_str << "]" << RESET
                  << " TOPIC: Channel " << cmd[1] << " does not exist" << std::endl;
        m_Server.sendResponse(ERR_NOSUCHCHANNEL(m_Server.getHostname(), m_Server.getClientByFd(fd)->getNick(), cmd[1]), fd);
        return ;
    }

    Client* client = m_Server.getClientByFd(fd);

    // view topic
    if (cmd.size() == 2) {
        std::string topic = channel->getTopic();

        if (topic.empty()) {
            std::cout << BLUE << "[" << time_str << "]" << RESET
                      << " TOPIC: No topic is set for " << channel->getName() << std::endl;
            m_Server.sendResponse(RPL_NOTOPIC(m_Server.getHostname(), client->getNick(), channel->getName()), fd);
        } else {
            std::cout << BLUE << "[" << time_str << "]" << RESET
                      << " TOPIC: Returning topic for " << channel->getName() << ": " << topic << std::endl;
            m_Server.sendResponse(RPL_TOPIC(m_Server.getHostname(), client->getNick(), channel->getName(), topic), fd);
        }
        return ;
    }
    // set topic
    else if (cmd.size() >= 3) {
        std::cout << BLUE << "[" << time_str << "]" << RESET
                  << " TOPIC: " << client->getNick() << " attempts to set topic for " << channel->getName()
                  << ": " << cmd[2] << std::endl;

        if (channel->getTopicPrivilege() && !channel->isOperator(client)) {
            std::cout << BLUE << "[" << time_str << "]" << RESET
                      << " TOPIC: Permission denied — " << client->getNick()
                      << " is not an operator and +t is set" << std::endl;
            m_Server.sendResponse(ERR_CHANOPRIVSNEEDED(m_Server.getHostname(), client->getNick(), channel->getName()), fd);
        } else {
            std::string newTopic = cmd[2];
            for (size_t i = 3; i < cmd.size(); ++i)
                newTopic += " " + cmd[i];

            if (!newTopic.empty() && newTopic[0] == ':')
                newTopic.erase(0, 1);

            channel->setTopic(newTopic);
            std::cout << BLUE << "[" << time_str << "]" << RESET
                      << " TOPIC: Topic for " << channel->getName() << " set to: " << cmd[2]
                      << " by " << client->getNick() << std::endl;
            m_Server.dispatchToChannel(
                channel,
                CMD_TOPIC(client->getNick(), client->getUsername(), client->getHostname(), channel->getName(), channel->getTopic()),
                NULL);
        }
        return ;
    } 
}
