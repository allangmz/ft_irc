#ifndef COMMANDE_HPP
# define COMMANDE_HPP

# include <iostream>
# include <string>
# include <map>

class Client;

class Server;

/* 
	list des commandes "USER" a traiter
	- NICK <nickname> : change le pseudo de l'utilisateur
	- USER <username> <nickname> : enregistre l'utilisateur sur le serveur
	- JOIN <channel> : rejoint un channel
	- PART <channel> : quitte un channel
	- PRIVMSG <receiver> <message> : envoie un message privé à un utilisateur ou un channel
	- NAMES <channel> : liste les utilisateurs d'un channel
	- LIST : liste les channels du serveur
	liste des commandes "ADMIN" a traiter
	- KICK <channel> <user> : kick un utilisateur d'un channel
	- INVITE <user> <channel> : invite un utilisateur dans un channel
	- TOPIC <channel> <topic> : change le topic d'un channel
	- MODE <channel> <mode> : change le mode d'un channel
		-i : set or remove the invite-only flag;
		-t : Set/remove the restrictions of the TOPIC command to channel operators;
		-k : Set/remove the channel key (password);
		-o : Give/remove channel operator privileges to <nickname>;
*/
	void	rplConnectionMessages(std::string msg, Client &client, Server &server);

	/* Commande connection user */

	void	connect_cap(std::string msg, Client &client, Server &server);
	void	connect_nick(std::string msg, Client &client, Server &server);
	void	connect_user(std::string msg, Client &client, Server &server);
	void	connect_pass(std::string msg, Client &client, Server &server);
	void	connect_ping(std::string msg, Client &client, Server &server);


	/* Commande User */

	void	join(std::string msg, Client &client, Server &server);
	void	list(std::string msg, Client &client, Server &server);
	void	names(std::string msg, Client &client, Server &server);
	void	nick(std::string msg, Client &client, Server &server);
	void	part(std::string msg, Client &client, Server &server);
	void	privmsg(std::string msg, Client &client, Server &server);
	void	user(std::string msg, Client &client, Server &server);
	void	quit(std::string msg, Client &client, Server &server);
	

	/* Commande Admin */

	void	topic(std::string msg, Client &client, Server &server);
	void	mode(std::string msg, Client &client, Server &server);
	void	invite(std::string msg, Client &client, Server &server);
	void	kick(std::string msg, Client &client, Server &server);

#endif