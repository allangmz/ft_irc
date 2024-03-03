# include "Server.hpp"

void Server::parseMessage(std::string msg, Client &client, Server &server)
{
    if (msg.empty())
        return;

	if (client.getNickname().empty())
		std::cout << "Client " << RED << "User in connection proccess" << RESET << " send command!" << std::endl;
	else
		std::cout << "Client " << RED << client.getNickname() << RESET << " send command! : " << msg<<std::endl;
	std::cout << "----------------------------------------------------------------------" << std::endl;
	std::cout << msg << std::endl;
	std::cout << "----------------------------------------------------------------------" << std::endl;

    if (msg.find("\r\n") == msg.npos)
        return (client.set_buffer(client.get_buffer() + msg));

	std::istringstream iss(client.get_buffer() + msg);
	std::string line, token_one, token_last;
	while (std::getline(iss, line, '\n'))
	{
		if (line.empty())
		{
			break ;
		}
		// std::istringstream iss2(client.get_buffer() + msg);
		std::istringstream iss2(line);
    	iss2 >> token_one >> token_last;
		if (commandMap.find(token_one) != commandMap.end())
			commandMap[token_one](line, client, server);
		else
    	    std::cout << "Command not found: " << token_one << std::endl;
	}
	client.set_buffer("");
}



/*

------------------------------------------------------------------------------------------
Commande connection User
------------------------------------------------------------------------------------------

*/

void	connect_cap(std::string msg, Client &client, Server &server)
{
	(void) server;
	std::istringstream iss(msg);
	std::string command, farg, sarg;
	iss >> command >> farg >> sarg; // Ignore le premier argument (CAP)
	if (farg == "LS" || (farg == "LS" && sarg == "302"))
	{	
		if (client.get_is_connected() == false)
		{
			std::string message = "CAP * LS :\r\n";
			send(client.get_socket(), message.c_str(), message.length(), 0);
			msg = "";
			client.set_buffer("");
			return ;
		}
		else
		{
			std::string message = ERR_ALREADYREGISTRED;
			send(client.get_socket(), message.c_str(), message.length(), 0);
			msg = "";
			client.set_buffer("");
			return ;
		}
	}
	else if (farg == "REQ")
	{
		std::string message = "CAP * ACK multi-prefix\r\n";
		send(client.get_socket(), message.c_str(), message.length(), 0);
		msg = "";
		client.set_buffer("");
		return ;
	}
	
	else if (farg == "END")
	{
		if (client.get_status() == 3)
		{
			client.set_is_connected();
			rplConnectionMessages(msg, client, server);
		}
		client.set_buffer("");
		return ;
	}
	// client.set_buffer(msg);
	return ;
}

void	connect_nick(std::string msg, Client &client, Server &server)
{
	if (!client.get_is_connected() && client.getNickname().empty())
	{
		std::istringstream iss(msg);
		std::string command, arg, msg;
		iss >> command >> arg; // Ignore le premier argument (NICK)
		std::list<Client> clientsList = server.get_clients();
		for (std::list<Client>::iterator it = clientsList.begin(); it != clientsList.end(); ++it)
		{
			// Vérification des correspondances exactes
			if (it->getNickname() == arg)
			{
				std::string message = ERR_NICKNAMEINUSE;
				send(client.get_socket(), message.c_str(), message.length(), 0);
				part(arg + " " + message.substr(0, message.length()), client, server);
				client.set_buffer("");
				return ;
			}
			else
			{
				if (arg[0] == '#')
					arg[0] = '+';
				client.setNickname(arg);
				client.set_status(client.get_status() + 1);
				if (client.get_status() == 3)
				{
					client.set_is_connected();
					rplConnectionMessages(msg, client, server);
				}
				client.set_buffer("");
				return ;
			}
		}
		
	}
	else
	{
		// - NICK <nickname> : change le pseudo de l'utilisateur
		std::istringstream iss(msg);
		std::string command, nickname, oldNick;
		iss >> command; 
		iss >> nickname;
		oldNick = client.getNickname();
		if (nickname.empty())
		{
			std::string message = ERR_NONICKNAMEGIVEN;
			send(client.get_socket(), message.c_str(), message.length(), 0);
			client.set_buffer("");
			return ;
		}
		if (oldNick == nickname)
		{
			std::string message = ERR_ALREADYREGISTRED;
			send(client.get_socket(), message.c_str(), message.length(), 0);
			client.set_buffer("");
			return ;
		}
		else
		{
			std::list<Client> clientsList = server.get_clients();
			for (std::list<Client>::iterator it = clientsList.begin(); it != clientsList.end(); ++it)
			{
				// Vérification des correspondances exactes
				if (it->getNickname() == nickname)
				{
					std::string message = ERR_NICKNAMEINUSE;
					send(client.get_socket(), message.c_str(), message.length(), 0);
					client.set_buffer("");
					return ;
				}
				// Vérification des correspondances partielles
				if (it->getNickname().find(nickname) != std::string::npos || nickname.find(it->getNickname()) != std::string::npos)
				{
					std::string message = ERR_NICKCOLLISION;
					send(client.get_socket(), message.c_str(), message.length(), 0);
					client.set_buffer("");
					return;
				}
			}
		}
		
		// :george!~b@localhost NICK :my-name-is-george-it-is-a-real
		std::string message = ":" + client.getNickname()+ "!~@" + SERV_NAME " NICK :" + nickname + "\r\n";
		client.setNickname(nickname);
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
	}
}

void	connect_pass(std::string msg, Client &client, Server &server)
{
	(void) server; 
	if (!client.get_is_connected() && client.get_password().empty())
	{
		std::istringstream iss(msg);
		std::string command, arg;
		iss >> command >> arg; // Ignore le premier argument (PASS)
		if (arg != SERV_PASS)
		{
			std::string message = ERR_PASSWDMISMATCH;
			send(client.get_socket(), message.c_str(), message.length(), 0);
			client.set_buffer("");
			msg = ERR_PASSWDMISMATCH;
			quit(msg, client, server);
			return ;
		}
		client.set_password(arg);
		client.set_status(client.get_status() + 1);
		if (client.get_status() == 3)
		{
			client.set_is_connected();
			rplConnectionMessages(msg, client, server);
			return ;
		}
		client.set_buffer("");
		return ;
	}
	else
	{
		std::string message = ERR_ALREADYREGISTRED;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		return ;
	}
}

void	connect_ping(std::string msg, Client &client, Server &server)
{
	(void) server;
	std::istringstream iss(msg);
	std::string command, arg, args;
	iss >> command >> arg >> args; // Ignore le premier argument (PING)
	if (arg == "")
	{
		std::string message = ERR_NEEDMOREPARAMS;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
	}
	else if (arg != "" && args == "")
	{

		std::string message = ":" SERV_NAME " PONG " SERV_NAME " " + arg + "\r\n";
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
	}
	else if (arg != "" && args != "")
	{
		std::string message = ":" SERV_NAME  " PONG " SERV_NAME " " + arg + " " + args + "\r\n";
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
	}
}







/*

------------------------------------------------------------------------------------------
Commande User
------------------------------------------------------------------------------------------

*/
void	rplConnectionMessages(std::string msg, Client &client, Server &server)
{
	(void) server;
	/* <client> :Welcome to the Internet Relay Network <nick>!<user>@ */
	std::string message = RPL_WELCOME;
	// std::string message = ":" SERV_NAME " 001 " + client.getNickname() + " :bonjour to the Internet Relay Network! " + client.getUsername() + "@" + "\r\n";
	send(client.get_socket(), message.c_str(), message.length(), 0);

	/* <client> :Your host is <servername>, running version <version> */
		
	message = RPL_YOURHOST;
	send(client.get_socket(), message.c_str(), message.length(), 0);

	/* <client> :This server was created <date> */
	message = RPL_CREATED;
	send(client.get_socket(), message.c_str(), message.length(), 0);

	/* <client> <server_name> <version> <usermodes> <chanmodes> [chanmodes_with_a_parameter] */
	message = RPL_MYINFO;
	send(client.get_socket(), message.c_str(), message.length(), 0);

	/* <client> <1-13 tokens> :are supported by this server */
	message = RPL_ISUPPORT ;
	send(client.get_socket(), message.c_str(), message.length(), 0);
	message = "";
	msg = "";
	client.set_buffer("");
	return ;
}

void	headerConnectionChannel(Channel &channel, Client &client, Server &server)
{
	(void) server;
	/* Message header du nouveau conecter */

	//:<nickname> JOIN <#channelName>
	std::string message = ":" + client.getNickname() + " JOIN #" + channel.getName() + "\r\n";
	send(client.get_socket(), message.c_str(), message.length(), 0);

	//:<ServName> 332 <nickname> <ChannelName> :<TopicName>
	message = ":" SERV_NAME " 332" + client.getNickname() + "#" + channel.getName() + channel.getTopic() + " :\r\n";
	send(client.get_socket(), message.c_str(), message.length(), 0);

	//fair une boucle foreach user dans iterator it de la list des clients du channel qui affichera les users present sur le cannal
	std::map<std::string,Client&>	clients = channel.getClientMap();
	std::map<std::string,Client&>::iterator it = clients.begin();
	std::map<std::string,Client&>::iterator ite = clients.end();
	int i = 0;
	for (; it != ite; it++)
	{
		//:<ServName> 353 <nickname> = <ChannelName> :<nickname> <nickname> <nickname> ...
		message = ":" SERV_NAME " 353 " + client.getNickname() + " = #" + channel.getName() + " :" + it->first + "\r\n";
		send(client.get_socket(), message.c_str(), message.length(), 0);
		i++;
	}

	//:<ServName> 366 <nickname> <ChannelName> :End of /NAMES list
	message = ":" SERV_NAME " 366 " + client.getNickname() + " #" + channel.getName() + " :End of /NAMES list\r\n";
	send(client.get_socket(), message.c_str(), message.length(), 0);
}

void	userConnectionChannel(Channel &channel, Client &client, Server &server)
{
	(void) server;

	std::map<std::string, Client&> clients = channel.getClientMap();
	std::map<std::string, Client&>::iterator it = clients.begin();
	std::map<std::string, Client&>::iterator ite = clients.end();
	
	for (; it != ite; it++)
	{
		if (it->second.getNickname() != client.getNickname())
		{
			std::string message = ":" + client.getNickname() + " JOIN #" + channel.getName() + "\r\n";
			send(it->second.get_socket(), message.c_str(), message.length(), 0);
		}
	}
	return ;
}

void	join(std::string msg, Client &client, Server &server)
{
	// - JOIN <channel> : rejoint un channel

	std::istringstream iss(msg);
	std::string command, farg, sarg, channelName, password;
	Channel * channel;
	iss >> command >> farg >> sarg ; // Ignore le premier argument (/join)

	if (client.get_is_connected() == false)
	{
		std::string message = ERR_NOTREGISTERED;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		return ;
	}
	if (farg == ":")
	{
		if (sarg == "")
		{
			std::string message = ERR_NEEDMOREPARAMS;
			send(client.get_socket(), message.c_str(), message.length(), 0);
			return ;
		}
		channelName = sarg;
	}
	else if (farg == "")
	{
		std::string message = ERR_NEEDMOREPARAMS;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		return ;
	}
	else 
		channelName = farg;

	// Vérification du "#" au premier caractère du channel
	if (!channelName.empty() && channelName[0] == '#')
	{
		channelName.erase(0,1);
		if (server.doesChannelExist(channelName))
			channel = &server.getChannel(channelName);
	}
	else
	{
		std::string message = ERR_NOSUCHCHANNEL;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		return ;
	}
	/* Si le Channel n'existe pas */
	if (server.doesChannelExist(channelName) == false)
	{
		// creer le chanel
		Channel *newChannel = new Channel(channelName);
		// ajouter le chanel dans la list des chanel
		server.addChannel(newChannel);
		// ajouter le chanel dans la list des channel du client et ajouter le client dans la list des clients du channel
		server.addClientToChannel(*newChannel, client);
		client.set_is_admin(true);
		client.addChannelToClient(*newChannel);
		newChannel->addOperator(client);
		/* Message header du nouveau conecter */
		headerConnectionChannel(*newChannel, client, server);
		/* Messages d'entree sur le serv d'un user pour tout les users */
		userConnectionChannel(*newChannel, client, server);


		client.set_buffer("");
		return ;
	}

	/* Si le channel existe*/
	/* Si le channel est full */
	if((size_t)channel->getUserLimit() < channel->getClientMap().size())
	{
		std::string message = ERR_CHANNELISFULL;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		return ;
	}

	/* si le channel existe et est en privé */
	else if (server.doesChannelExist(channelName) && channel->getInviteOnlyStatus()== true)
	{
		std::list<Client> invitedClients = channel->getInvitedClients();
		bool is_client_invited = false;
		// verifier si le client est dans la list des clients du chanel
		for (std::list<Client>::iterator it = invitedClients.begin(); it != invitedClients.end(); it++)
		{
			if (it->getNickname() == client.getNickname())
				is_client_invited = true;
		}
		if (is_client_invited)
		{
			server.addClientToChannel(*channel, client);
			client.addChannelToClient(*channel);
			/* Message header du nouveau conecter */
			headerConnectionChannel(*channel, client, server);

		}
		else
		{
			std::string message = ERR_INVITEONLYCHAN;
			send(client.get_socket(), message.c_str(), message.length(), 0);
			std::cout << "Channel on invitation only mode" << std::endl;
		}	
			
		
	}

	/* si le channel existe et n'est pas en privé */
	else if (server.doesChannelExist(channelName) && channel->getInviteOnlyStatus() == false) // NEW
	{
		//join channel
		server.addClientToChannel(*channel, client);
		client.addChannelToClient(*channel);

		/* Message header du nouveau conecter */
		headerConnectionChannel(*channel, client, server);
		userConnectionChannel(*channel, client, server);
	}


	else
		std::cout << "Channel on invitation only mode" << std::endl; // NEW a changer
	
	/* affiche tous les membres du channel */
	// if (server.doesChannelExist(channelName))
	// {
	// 	Channel channel = *channel;
	// 	std::cout<< "Channel name = " << channel.getName() << std::endl;
	// 	std::map<std::string,Client&>	clients =channel.getClientMap();

	// 	if(clients.size()==0)
	// 	{
	// 		std::cout << "No clients in channel" << std::endl;
	// 		return ;
	// 	}
	// 	std::map<std::string,Client&>::iterator it = clients.begin();
	// 	std::map<std::string,Client&>::iterator ite = clients.end();
	// 	for (; it != ite; it++)
	// 	{
	// 		std::cout << it->first << std::endl;
	// 	}
	// }
	return ;
}

void	list(std::string msg, Client &client, Server &server)
{
	// - LIST : liste les channels du serveur
	(void) msg;
	// afficher la list des chanel du serv
	std::cout << "List of channels:" << std::endl;

	std::map<std::string, Channel*> channelMap = server.getChannelMap();
	std::map<std::string, Channel*>::iterator it = channelMap.begin();
	std::map<std::string, Channel*>::iterator ite = channelMap.end();
	for (; it != ite; ++it)
	{
		std::string message = it->first + "\n";
		send(client.get_socket(), message.c_str(), message.length(), 0);
		// std::cout << it->first << std::endl;
	}
}

void	names(std::string msg,Client &client, Server &server)
{
	(void) client;
	// - NAMES <channel> : liste les utilisateurs d'un channel

	std::istringstream iss(msg);
	std::string command, sep, channelName;
	iss >> command >> sep >> channelName; // Ignore le premier argument (/NAMES)


	/*
	if (chanel existe)
		verfier si le channel existe
		afficher le nom des user du chanel

	else
		afficher un message d'erreur
		std::cout << "Channel not found" << std::endl;
	*/
	if (server.doesChannelExist(channelName))
	{
		Channel &channel = server.getChannel(channelName);
		std::map<std::string,Client&> clientMap = channel.getClientMap();
		for (std::map<std::string, Client&>::iterator it = clientMap.begin(); it != clientMap.end(); ++it)
		{
			std::cout << it->first << std::endl;
		}
	}
	else
		std::cout << "Channel not found" << std::endl;
}

void	part(std::string entry, Client &client, Server &server)
{
	// - PART <channel> : quitte un channel
	std::istringstream iss(entry);
	std::string command, channelName, message;
	iss >> command >> channelName >> message; // Ignore le premier argument (/PART)
	if (channelName.empty() || channelName == command)
	{
		std::string message = ERR_NEEDMOREPARAMS;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return ;
	}
	channelName.erase(0,1);
	if (server.doesChannelExist(channelName))
	{
		Channel &channel = server.getChannel(channelName);
		std::map<std::string, Client&> clients = channel.getClientMap();
		std::map<std::string, Client&>::iterator it = clients.begin();
		std::map<std::string, Client&>::iterator ite = clients.end();
		for (; it != ite; it++)
		{
			// send to everyone
			//commande part
			message = ":" + client.getNickname() + " PART #" + channel.getName() + "\r\n";
			send(it->second.get_socket(), message.c_str(), message.length(), 0);
		}
		channel.removeClient(client);
		client.removeChannelFromClient();
		if (channel.getClientMap().empty())
		{
			server.removeChannel(channelName);
			client.set_buffer("");
			return ;
		}
		if (channel.isOperator(client))
		{
			channel.removeOperator(client);
			if(!channel.isOperator(channel.getClientMap().begin()->second))
			{
				channel.addOperator(channel.getClientMap().begin()->second);
			}
		}	
		client.set_buffer("");
		return ;
	}
	else
	{
		std::string message = ERR_NOSUCHCHANNEL;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return ;
	}
}

void	privmsg(std::string msg, Client &client, Server &server)
{
	// - PRIVMSG <receiver> <message> : envoie un message privé à un utilisateur ou un channel


	std::istringstream iss(msg);
	std::string command, receiver, message;
	iss >> command >> receiver; // Ignore le premier argument (/PRIVMSG)
	message = msg.substr(msg.find(receiver) + receiver.length() + 2);

	if (receiver.empty())
	{
		message = ERR_NORECIPIENT;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return ;
	}


	// Vérification du "#" au premier caractère du receiver
	if (!receiver.empty() && receiver[0] == '#')
	{
		receiver.erase(0,1);
		if (!client.getChannel() || client.getChannel()->getName() != receiver)
		{
			message = ERR_CANNOTSENDTOCHAN;
			send(client.get_socket(), message.c_str(), message.length(), 0);
			client.set_buffer("");
			return ;
		}
		if (!server.doesChannelExist(receiver))
		{
			message = ERR_NOSUCHCHANNEL;
			send(client.get_socket(), message.c_str(), message.length(), 0);
			client.set_buffer("");
			return ;
		}
		std::map<std::string, Channel*> channelMap = server.getChannelMap();
		std::map<std::string, Channel*>:: iterator it =  channelMap.begin();
		std::map<std::string, Channel*>:: iterator ite = channelMap.end();
		for (; it != ite; it++)
		{
			if (it->second->getName() == receiver)
			{

				/* Format du message = ":brissboss PRIVMSG #test :salut" */
				it->second->putMessageChannel(message, client, server.temp_write_fds);
				client.set_buffer("");
				return ;
			}
		}
		std::string message = ERR_NOSUCHNICK;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return;
	}

	else
	{
		if (!server.doesClientExist(receiver))
		{
			message = ERR_NOSUCHNICK;
			send(client.get_socket(), message.c_str(), message.length(), 0);
			client.set_buffer("");
			return ;
		}
		//enlever le \n\r a la fin de la string
		if (message.find("\r") != std::string::npos)
			message.erase(message.find("\r"), 1);
		server.putMessage(message, client, receiver, server.temp_write_fds);
	}
}

bool	containsAlphabeticCharacters(const std::string& str)
{
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
	{
		if (std::isalpha(static_cast<unsigned char>(*it)))
		{
			return true;
		}
	}
	return false;
}

void 	user(std::string msg, Client &client, Server &server)
{
	// - USER <username> <nickname> : enregistre l'utilisateur sur le serveur
	(void) server;
	if (!client.get_is_connected() && client.getUsername().empty())
	{
		std::istringstream iss(msg);
		std::string command, username, voidParam, ipParam, realname;

		// Parse the message to extract the relevant parts
		iss >> command >> username >> voidParam >> ipParam;

		// Get the position of the first ":" in the remaining message
		std::size_t pos = msg.find(':');
		if (pos != std::string::npos)
		{
			realname = msg.substr(pos + 1);

			// Check if the realname contains alphabetic characters
			bool hasAlphabeticCharacters = containsAlphabeticCharacters(realname);

			if (!hasAlphabeticCharacters)
			{
				std::string message = ERR_ERRONEUSNICKNAME;
				send(client.get_socket(), message.c_str(), message.length(), 0);
				return;
			}
		}
		else
		{
			std::string message = ERR_NEEDMOREPARAMS;
			send(client.get_socket(), message.c_str(), message.length(), 0);
			return;
		}
		client.setUsername(username);
		client.set_ip(ipParam);
		client.setRealname(realname);

		client.set_status(client.get_status() + 1);
		if (client.get_status() == 3)
		{
			client.set_is_connected();
			rplConnectionMessages(msg, client, server);
		}
		client.set_buffer("");
		return ;
	}
	else
	{
		std::string message = ERR_ALREADYREGISTRED;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return;
	}
}

void	quit(std::string msg, Client &client, Server &server)
{
	std::istringstream iss(msg);
	std::string command, message;
	if (msg.find(':') != std::string::npos)
		message = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.get_ip() + " " + msg + "\r\n";
	else
		message = ":" + client.getNickname() + "!" + client.getUsername() + "@" + client.get_ip() + " QUIT\r\n";
	if (client.getChannel())
		part("PART #"+ client.getChannel()->getName(), client, server);
	client.set_is_disconnected();
	FD_CLR(client.get_socket(), &server._read_fds);
	FD_CLR(client.get_socket(), &server._write_fds);
	server.removeClient(client);
	server.set_max_fd(server.get_max_fd() - 1);
}

/*

------------------------------------------------------------------------------------------
commandes admin
------------------------------------------------------------------------------------------

*/

void kick(std::string msg, Client &client, Server &server)
{
	std::istringstream iss(msg);
	std::string command, channelName, receiver, reason, message;
	iss >> command;
	if (!(iss>>channelName) || !(iss>>receiver))
	{
		message = ERR_NEEDMOREPARAMS;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return;
	}
	if (!(iss>>reason))
		reason = "";
	channelName = channelName.substr(1);
	if (!server.doesChannelExist(channelName))
	{
		message = ERR_NOSUCHCHANNEL;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return;
	}
	Channel *channel = &server.getChannel(channelName);
	if (!channel->isInChannel(client))
	{
		message = ERR_NOTONCHANNEL;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return;
	}
	if (!channel->isOperator(client))
	{
		message = ERR_CHANOPRIVSNEEDED;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return;	
	}
	else if (!server.doesClientExist(receiver))
	{
		message = ERR_NOSUCHNICK;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return;
	}
	Client &target = server.FindClient(receiver);
	if (!channel->isInChannel(target))
	{
		message = ERR_USERNOTINCHANNEL;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return;	
	}
	else if (!channel->isOperator(client))
	{
		message = ERR_CHANOPRIVSNEEDED;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return;	
	}
	else
		message = ":" + client.getNickname() + " KICK #" + channelName + " " + receiver + " " + reason + "\r\n";
	channel->sendCommandToAll(message, server.temp_write_fds);
	client.set_buffer("");
	return;
}

std::vector< std::string >	splitParameters(std::string content, std::string delimiter)
{
    std::vector< std::string >  words;
    std::string                 word;

    size_t                      pos = 0;

    while ((pos = content.find(delimiter)) != std::string::npos) {
        if (content.front() == ':')
            break;
        word = content.substr(0, pos);
        words.push_back(word);
        content.erase(0, pos + delimiter.length());
    }

    words.push_back(content);
    return words;
}

void	kill(std::string msg, Client &client, Server &server)
{
	// - KILL <user> <message> : kick un utilisateur du serveur
	
	std::vector< std::string > params = splitParameters(msg, " ");
	std::string message;

	if (params.size() < 2)
	{
		message = ERR_NEEDMOREPARAMS;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return ;
	}

	if (!client.get_is_admin())
	{
		message = ERR_NOPRIVILEGES;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return ;
	}

	std::map<std::string, Client&> mapClients = client.getChannel()->getClientMap();
	std::map<std::string, Client&>::iterator it = mapClients.begin();
	std::map<std::string, Client&>::iterator ite = mapClients.end();
	// message = params[2];
	for (; it != ite; it++)
	{
		if (it->first == params[1])
		{
			message = ":" + client.getNickname() + " KILL " + it->first + " :" + message + "\r\n";
			part((*it).first + " " + message.substr(0, message.length()), client, server);
			client.set_is_disconnected();
			break ;
		}
	}

	if (it == ite)
	{
		// message = ERR_NOSUCHNICK;
		// send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return ;
	}
}

void	invite(std::string msg, Client &client, Server &server)
{
	std::string message;
	/*
		if client exists and is connected
		send invite that client can accept or reject
		if client is in a channel remove him from the first and put him in the second
	*/
	std::istringstream iss(msg);
	std::string command, receiver, channelName;
	iss >> command;
	if (!(iss>>receiver) || !(iss>>channelName))
	{
		message = ERR_NEEDMOREPARAMS;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return ;
	}
	if (channelName[0]== '#')
		channelName = channelName.substr(1);
	else
	{
		message = ERR_NEEDMOREPARAMS;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return ;
	}

	if (server.doesClientExist(receiver))
	{
		if (server.doesChannelExist(channelName))
		{
			Client &client_to_invite = server.getClientFromNickname(receiver);
			Channel &channel = server.getChannel(channelName);
			if (!channel.isOperator(client))
			{
				message = ERR_CHANOPRIVSNEEDED;
				send(client.get_socket(), message.c_str(), message.length(), 0);
				client.set_buffer("");
				return ;
			}
			//ajouter client sur une liste dinvitation du channel
			if (client_to_invite.get_is_connected() == false)
			{
				message = ERR_NOSUCHNICK;
				send(client.get_socket(), message.c_str(), message.length(), 0);
				client.set_buffer("");
				return ;
			}
			
			message = RPL_INVITELIST;
			server.putMessage(message,client , receiver, server.temp_write_fds);
			message = RPL_INVITED;
			channel.addClientToInvitationList(client_to_invite);
			channel.sendCommandToAll(message, server.temp_write_fds);
		}
		else
		{
			message = ERR_NOSUCHCHANNEL;
			send(client.get_socket(), message.c_str(), message.length(), 0);
		}
	}
	else
	{
		message = ERR_NOSUCHNICK;
		send(client.get_socket(), message.c_str(), message.length(), 0);
	}
	client.set_buffer("");
	return;
}

void	topic(std::string msg, Client &client, Server &server)
{
	std::istringstream iss(msg);
	std::string message;
	std::string channelName;
	std::string new_topic;

	iss>>message>>channelName;
	/*
		if channel exists
		if client is op
		print topic
	*/
	channelName = channelName.substr(1, channelName.length());
	Channel *channel = &server.getChannel(channelName);
	if (channel == nullptr || channel->getName() != client.getChannel()->getName())
	{
		message = ERR_NOTONCHANNEL;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return;
	}
	if(channel->getTopicStatus() == true && channel->isOperator(client) == false)
	{
		message = ERR_CHANOPRIVSNEEDED;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return;
	}
	else if(!(iss>>new_topic))
	{
		if (channel->getTopic() == "")
			message = RPL_NOTOPIC;
		else
			message = RPL_TOPIC;
		channel->sendCommandToAll(message,server.temp_write_fds);
		client.set_buffer("");
		return;
	}
	else
	{
		channel->setTopic(new_topic);

		message = RPL_TOPIC;
		channel->sendCommandToAll(message,server.temp_write_fds);
	}
	client.set_buffer("");
	return; 
}

void	mode(std::string msg, Client &client, Server &server )
{
	// MODE - Changer le mode du channel :
	// 	— i : Définir/supprimer le canal sur invitation uniquement
	// 	— t : Définir/supprimer les restrictions de la commande TOPIC pour les opé-
	// 					rateurs de canaux
	// 	— k <password>: Définir/supprimer la clé du canal (mot de passe)
	// 	— o <nickname>: Donner/retirer le privilège de l’opérateur de canal
	// 	— l <integer>: Définir/supprimer la limite d’utilisateurs pour le canal

	std::istringstream iss(msg);
	std::string command, channelName, mode, parameter, message, receiver;
	iss >> command;
	parameter = "";
	if(!(iss >> channelName) || !(iss >> mode))
	{
		message = ERR_NEEDMOREPARAMS;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return ;
	}
	else if(channelName[0] != '#')
	{
		message = ERR_NOSUCHCHANNEL;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return ;
	}
	channelName = channelName.substr(1, channelName.length());
	if(!server.doesChannelExist(channelName))
	{
		message = ERR_NOSUCHCHANNEL;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return ;
	}
	Channel &channel = server.getChannel(channelName);
	if (!channel.isInChannel(client))
	{
		message = ERR_NOTONCHANNEL;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return ;
	}
	else if (!channel.isOperator(client))
	{
		message = ERR_CHANOPRIVSNEEDED;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return ;
	}
	if (mode == "-i" )
	{
		if (channel.getInviteOnlyStatus() == true)
			channel.setInviteOnlyStatus(false);
		else
			channel.setInviteOnlyStatus(true);
	}
	else if (mode == "-t")
	{
		if (channel.getTopicStatus() == true)
			channel.setTopicStatus(false);
		else
			channel.setTopicStatus(true);
	}
	else if (mode == "-k")
	{
		if (!(iss >> parameter))
		{
			message = ERR_NEEDMOREPARAMS;
			send(client.get_socket(), message.c_str(), message.length(), 0);
			client.set_buffer("");
			return ;
		}
		else
			channel.setPassword(parameter);
	}
	else if (mode == "-o")
	{
		if (!(iss >> parameter))
		{
			message = ERR_NEEDMOREPARAMS;
			send(client.get_socket(), message.c_str(), message.length(), 0);
			client.set_buffer("");
			return ;
		}
		else if (!server.doesClientExist(parameter))
		{
			receiver = parameter;
			message = ERR_NOSUCHNICK;
			send(client.get_socket(), message.c_str(), message.length(), 0);
			client.set_buffer("");
			return ;
		}
		else if (!channel.isInChannel(server.getClientFromNickname(parameter)))
		{
			receiver = parameter;
			message = ERR_USERNOTINCHANNEL;
			send(client.get_socket(), message.c_str(), message.length(), 0);
			client.set_buffer("");
			return ;
		}
		else if (channel.isOperator(server.getClientFromNickname(parameter)))
		{
			message = ERR_CHANOPRIVSNEEDED;
			send(client.get_socket(), message.c_str(), message.length(), 0);
			client.set_buffer("");
			return ;
		}
		else
			channel.addOperator(server.getClientFromNickname(parameter));
	}
	else if (mode == "-l")
	{
		if (!(iss >> parameter))
		{
			channel.setUserLimit(-1);
		}
		else
		{
			int limit = std::stoi(parameter);
			if (limit < 0)
			{
				message = ERR_INVALIDMODEPARAM;
				send(client.get_socket(), message.c_str(), message.length(), 0);
				client.set_buffer("");
				return ;
			}
			channel.setUserLimit(limit);
		}
	}
	else
	{
		message = ERR_UMODEUNKNOWNFLAG;
		send(client.get_socket(), message.c_str(), message.length(), 0);
		client.set_buffer("");
		return ;
	}
	message = "MODE #" + channelName + " " + mode + " " + parameter+ "\r\n";
	channel.sendCommandToAll(message, server.temp_write_fds);
	
	client.set_buffer("");
	return;
	




}


// MODE #CHANNEL USER OPTION ARGUMENT