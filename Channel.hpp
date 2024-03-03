#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <string>
# include <map>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <sys/types.h>
# include <unistd.h>


class Client;

class Channel
{
	public:

		Channel();
		Channel(std::string name);
		~Channel();
		Channel &operator=(const Channel &rhs);
		Channel(const Channel &rhs);

		void		setName(std::string name);
		std::string	getName() const;

		void		addOperator(Client &client);
		void		removeOperator(Client &client);
		bool		isOperator(Client &client) const;

		void		addClient(Client &client);
		void		removeClient(Client &client);
		void		kickClient(Client &client);
		void		inviteClient(Client &client);

		void		topicView();
		std::string	getTopic() const;
		void		setTopic(std::string topic);

		bool		isInChannel(Client &client) const;

		void		modeView(Client &client);

		void		putMessageChannel(std::string message, Client &client, fd_set &write_fds);
		void		sendCommandToAll(std::string message, fd_set &write_fds);


		void		operatorsSet(Client &client);

		bool		getInviteOnlyStatus();
		void		setInviteOnlyStatus(bool status);

		bool		getTopicStatus();
		void		setTopicStatus(bool status);

		bool		getPasswordStatus();
		void		setPasswordStatus(bool status);

		std::string	getPassword() const;
		void 		setPassword(std::string password);

		int			getUserLimit();
		void		setUserLimit(int limit);

		std::map<std::string, Client &>	getClientMap() const;
		std::map<std::string, Client &>	getOperatorMap() const;
		std::list<Client>				getInvitedClients() const;
		void							addClientToInvitationList(Client &client);


	private:

		std::string			_name;
		std::string			_password;
		std::string			_topic;
		bool				_isInviteOnly;
		bool				_isPasswordProtected;
		bool				_isTopicRestricted;
		int					_userLimit;

		std::map<std::string,Client&>	_clients;
		std::map<std::string,Client&>	_operators;
		std::list<Client>				_invitedClients;

};

#endif
 
