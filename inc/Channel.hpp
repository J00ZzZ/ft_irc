#pragma once
#ifndef _channel_hpp_
# define _channel_hpp_

# include <vector>
# include <string>
# include <algorithm>
# include <fstream>

class Client;
class Server;
class Bot;

class Channel {
	private:
		Server						*m_server;
		std::vector<Client *>		m_clients;
		std::vector<Client *>		m_operators;
		std::vector<Client *>		m_bannedClients;
		std::vector<Client *> 		m_invitedClients;
		std::string					m_name;
		std::string					m_topic;
		std::string					m_key;
		std::size_t					m_memberLimit;
		bool						m_inviteOnly;
		bool						m_topicOperatorPrivilege; // -> true: only OPs can change the topic, false: whoever
		std::string					m_cache_filename;

	public:
		Channel(Server *, const std::string &);
		Channel(const Channel &);
		~Channel(void);

		Channel	&operator = (const Channel &);

		/* SECTION: client operations on channel */
		void						addClient(Client *);
		void 						addClientToInvites(Client *c);
		void						rmClientFromInvites(Client *c);
		void						delClient(Client *);
		void						addOperator(Client *);
		void						delOperator(Client *);

		/* SECTION: setters */
		void 						toggleInviteOnly(void);
		void 						toggleTopicPrivilege(void);
		void						setTopic(std::string);
		void						setKey(std::string);
		void						setMemberLimit(std::size_t);

		/* SECTION: getters / checkers */
		static bool 				isValidName(std::string&);
		bool						isInviteOnly(void) const;
		bool						isBanned(Client *) const;
		bool 						isInvited(Client * c) const;
		bool						checkKey(std::string& ) const;
		bool						isChannelFull(void) const;
		bool						isOperator(Client *) const;
		bool						isClientOnChannel(Client *) const;

		const std::string			&getName(void) const;
		const std::vector<Client *> &getClients(void) const;
		const std::string 			&getTopic(void) const;
		const std::string			&getKey(void) const;
		const std::string			&getCacheFilename(void) const;
		bool						getTopicPrivilege(void) const;
		std::size_t					getMemberLimit(void);

		/* SECTION: Cache */
		void						writeToCache(const std::string &cached) const;
		void						forwardChanCache(const int fd) const;
};	

#endif
