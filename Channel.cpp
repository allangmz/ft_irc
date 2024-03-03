# include "Server.hpp"

Channel::Channel() : _name(""), _password(""), _topic("None"), _isInviteOnly(false), _isPasswordProtected(false), _isTopicRestricted(false), _userLimit(-1), _clients(), _operators(), _invitedClients()
{
	std::cout << "Channel: Default channel created." << std::endl;
}

Channel::Channel(std::string name) : _name(name), _password(""), _topic("None"), _isInviteOnly(false), _isPasswordProtected(false), _isTopicRestricted(false), _userLimit(-1), _clients(), _operators(), _invitedClients()
{
	std::cout << "Channel: " << name << " created." << std::endl;
}

Channel::~Channel()
{
	std::cout << "Channel: " << this->_name << " destroyed." << std::endl;
}

Channel &Channel::operator=(const Channel &rhs)
{
	if (this != &rhs)
	{
		this->_name = rhs._name;
		this->_password = rhs._password;
		this->_topic = rhs._topic;
		this->_isInviteOnly = rhs._isInviteOnly;
		this->_isTopicRestricted = rhs._isTopicRestricted;
		this->_userLimit = rhs._userLimit;
		this->_clients = rhs._clients;
		this->_operators = rhs._operators;
		this->_invitedClients = rhs._invitedClients;
	}
	return (*this);
}

Channel::Channel(const Channel &rhs) : _name(rhs._name), _password(rhs._password), _topic(rhs._topic), _isInviteOnly(rhs._isInviteOnly), _isPasswordProtected(false), _isTopicRestricted(rhs._isTopicRestricted), _userLimit(rhs._userLimit), _clients(rhs._clients), _operators(rhs._operators)
{
	*this = rhs;
}

void	Channel::setName(std::string name)
{
	this->_name = name;
}

std::string	Channel::getName() const
{
	return (this->_name);
}

void	Channel::addOperator(Client &client)
{
	this->_operators.insert(std::pair<std::string, Client&>(client.getNickname(), client));
}

void	Channel::addClient(Client &client)
{
	if (this->_userLimit > 0 && this->_clients.size() >= (unsigned long int)this->_userLimit) //NEW
	{
		std::cout << "Channel: " << this->_name << " is full." << std::endl;// a modifier
		return ;
	}
	this->_clients.insert(std::pair<std::string, Client&>(client.getNickname(), client));
}

void Channel::removeClient(Client &client)
{
	std::map<std::string, Client&>::iterator it = this->_clients.find(client.getNickname());
	if (it == this->_clients.end())
	{
		std::cout <<GREEN<< "Channel: " << this->_name << " does not contain " << client.getNickname() << RESET <<std::endl;
		return ;
	}	
	this->_clients.erase(it);
}

void Channel::removeOperator(Client &client)
{
	std::map<std::string, Client&>::iterator it = this->_operators.find(client.getNickname());
	if(it == this->_operators.end())
	{
		std::cout <<GREEN<< "Channel OPERATOR: " << this->_name << " does not contain " << client.getNickname() << RESET <<std::endl;
		return ;
	}
	this->_operators.erase(it);
	return;

}

void	Channel::kickClient(Client &client)
{

	if (isOperator(client))
	{

		this->removeOperator(client);
	}

	std::map<std::string, Client&>::iterator it = this->_clients.find(client.getNickname());

	this->_clients.erase(it);

	//delete channel if empty
	if (this->_clients.empty())
		delete(this);

}

void	Channel::inviteClient(Client &client)
{
	this->_clients.insert(std::pair<std::string, Client&>(client.getNickname(), client));
}

std::string	Channel::getTopic() const
{
	return (this->_topic);
}

void	Channel::setTopic(std::string topic)
{
	this->_topic = topic;
}

void	Channel::putMessageChannel(std::string message, Client &client, fd_set &write_fds)
{
	//send message to all clients in channel
	std::map<std::string, Client&>::iterator it = this->_clients.begin();
	std::map<std::string, Client&>::iterator ite = this->_clients.end();
	if (message.find("\r") != std::string::npos)
		message.erase(message.find("\r"), 1);
	for (; it != ite; it++)
	{
		if (it->second.getNickname() != client.getNickname())
		{
			if (FD_ISSET(it->second.get_socket(), &write_fds))
			{
				it->second.printMessage(":" + client.getNickname() + " PRIVMSG #" + this->_name + " :" + message + " \r\n");
				// this->_name ou getChannelName()??
				FD_CLR(it->second.get_socket(), &write_fds);
			}
		}
	}
	return ;
}

void		Channel::sendCommandToAll(std::string command, fd_set &write_fds)
{
	std::map<std::string, Client&>::iterator it = this->_clients.begin();
	std::map<std::string, Client&>::iterator ite = this->_clients.end();
	for (; it != ite; it++)
	{
		if (FD_ISSET(it->second.get_socket(), &write_fds))
		{
			it->second.printMessage(command);
			FD_CLR(it->second.get_socket(), &write_fds);
		}
	}
}

std::string Channel::getPassword() const
{
	return (this->_password);
}

void	Channel::setPassword(std::string password)
{
	this->_password = password;
}

std::map<std::string,Client&>	Channel::getClientMap() const
{
	return (this->_clients);
}

std::map<std::string, Client &>	Channel::getOperatorMap() const
{
	return (this->_operators);
}

bool	Channel::isOperator(Client &client) const
{
	std::map<std::string, Client&> operatorsMap = this->getOperatorMap();
	if (operatorsMap.size() == 0)
	{
		return false;
	}
    // std::map<std::string, Client&> operatorsMap = this->getOperatorMap();
    std::map<std::string, Client&>::iterator it = operatorsMap.begin();
	std::map<std::string, Client&>::iterator ite = operatorsMap.end();
	for (;it != ite; it++)
	{
    	if (it != operatorsMap.end() && (it->second.getNickname() == client.getNickname()))
		{
        	return true;
		}
	}
	return false;
	// std::map<std::string, Client&>::iterator it = this->_operators.find(client.get_name());
	// if (it != this->_operators.end())
	// {
	// 	return (true);
	// }
	// return (false);
}

bool		Channel::getInviteOnlyStatus()// NEW
{
	return (this->_isInviteOnly);
}

void		Channel::setInviteOnlyStatus(bool status)
{
	this->_isInviteOnly = status;
}

bool		Channel::getTopicStatus()
{
	return (this->_isTopicRestricted);
}

void		Channel::setTopicStatus(bool status)
{
	this->_isTopicRestricted = status;
}

bool	Channel::getPasswordStatus()
{
	return (this->_isPasswordProtected);
}

void	Channel::setPasswordStatus(bool status)
{
	this->_isPasswordProtected= status;
}

int 	Channel::getUserLimit()
{
	return (this->_userLimit);
}

void	Channel::setUserLimit(int limit)
{
	this->_userLimit = limit;
}

std::list<Client>	Channel::getInvitedClients() const
{
	return (this->_invitedClients);
}

void	Channel::addClientToInvitationList(Client &client)
{
	this->_invitedClients.push_back(client);
}

bool	Channel::isInChannel(Client &client) const
{
	std::map<std::string, Client&>::const_iterator it = this->_clients.find(client.getNickname());
	if (it != this->_clients.end())
		return (true);
	return (false);
}