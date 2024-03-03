#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <fstream>
# include <sstream>
# include <string>
# include <stdio.h>
# include <unistd.h>
# include <vector>
# include <map>
# include <list>
# include <sys/socket.h>
# include <sys/select.h>
# include <sys/event.h>
# include <sys/time.h>
# include <arpa/inet.h>
# include <algorithm>
# include <fcntl.h>

# define RESET "\033[0m"
# define RED "\033[1;31m"
# define GREEN "\033[1;32m"
# define YELLOW "\033[1;33m"
# define BLUE "\033[1;34m"
# define CYAN "\033[1;36m"

# define SERV_NAME "IRC_Server"
# define SERV_PASS "pass"

// std::string serveur_name = "IRC_Server";

/* ENUM Log */
# define RPL_WELCOME ":" SERV_NAME " 001 " + client.getNickname() + " :Welcome to the Internet Relay Network!" + client.getUsername() + "@" + "\r\n"
# define RPL_YOURHOST ":" SERV_NAME " 002 " + client.getNickname() + " :Your host is LocalHost\r\n"
# define RPL_CREATED ":" SERV_NAME " 003 " + client.getNickname() + " :This server was created by \"TEAM bras cassé\" the 25.07.2023\r\n"
# define RPL_MYINFO ":" SERV_NAME " 004 " + client.getNickname() + " :" SERV_NAME " 1.0\r\n"
# define RPL_ISUPPORT ":" SERV_NAME " 005 " + client.getNickname() + " :CHANTYPES=#&\r\n"

/* ENUM Username */

# define ERR_NONICKNAMEGIVEN ":" SERV_NAME " 431 :No nickname given\r\n"
# define ERR_ERRONEUSNICKNAME ":" SERV_NAME " 432 " + client.getNickname() + " :Erroneus nickname\r\n"
# define ERR_NICKNAMEINUSE ":" SERV_NAME " 433 " + client.getNickname() + " :Nickname is already in use\r\n"
# define ERR_NICKCOLLISION ":" SERV_NAME " 436 " + client.getNickname() + " :Nickname collision KILL\r\n"
# define ERR_NOSUCHNICK ":" SERV_NAME " 401 " + client.getNickname() +" "+ receiver + " :No such nick/channel\r\n"
# define RPL_PONG ":" SERV_NAME " PONG " + client.getNickname() + " " + client.getNickname() + "\r\n"

/* ENUM Error */

# define ERR_NORECIPIENT ":" SERV_NAME " 411 :No recipient given (" + client.getNickname() + ")\r\n"
# define ERR_NEEDMOREPARAMS ":" SERV_NAME " 461 " + client.getNickname() + ":Not enough parameters\r\n"
# define ERR_ALREADYREGISTRED ":" SERV_NAME " 462 :Unauthorized command (already registered)\r\n"
# define ERR_CHANOPRIVSNEEDED ":" SERV_NAME " 482 " + client.getNickname() + " #" + channelName + " :You're not channel operator\r\n"
# define ERR_USERSDONTMATCH ":" SERV_NAME " 502 :Cant change mode for other users\r\n"
# define ERR_NOTONCHANNEL ":" SERV_NAME " 442 " + client.getNickname() + " :You're not on that channel\r\n"
# define ERR_NOSUCHCHANNEL ":" SERV_NAME " 403 " + client.getNickname() + " :No such channel\r\n"
# define ERR_NOTREGISTERED ":" SERV_NAME " 451 :You have not registered\r\n"
# define ERR_NOPRIVILEGES ":" SERV_NAME " 481 :Permission Denied- You're not an IRC operator\r\n"
# define ERR_PASSWDMISMATCH ":" SERV_NAME " 464 :Password incorrect\r\n"
# define ERR_CANNOTSENDTOCHAN ":" SERV_NAME " 404 " + client.getNickname() + " :Cannot send to channel\r\n"
# define ERR_TOOMANYCHANNELS ":" SERV_NAME " 405 " + client.getNickname() + " :You have joined too many channels\r\n"
# define ERR_INVALIDMODEPARAM ":" SERV_NAME " 501 " + channel.getName() + " " + mode + " " + parameter + " :Unknown MODE parameter\r\n" 
# define ERR_UMODEUNKNOWNFLAG ":" SERV_NAME " 472 " + client.getNickname() + " " + mode + " :is unknown mode char to me\r\n"
# define ERR_CHANNELISFULL ":" SERV_NAME " 471 " + client.getNickname() + " #" + channelName + " :Cannot join channel (+l)\r\n"
# define ERR_USERONCHANNEL ":" SERV_NAME " 443 " + client.getNickname() + " " + receiver + " :is already on channel\r\n"
# define ERR_INVITEONLYCHAN ":" SERV_NAME " 473 " + client.getNickname() + " #" + channelName + " :Cannot join channel (+i)\r\n"

# define RPL_NOTOPIC ":" SERV_NAME " 331 " + client.getNickname() + " :No topic is set\r\n"
# define RPL_TOPIC ":" + client.getNickname() + " TOPIC #" + channel->getName() + " " + channel->getTopic() + "\r\n"
# define ERR_USERNOTINCHANNEL ":" SERV_NAME " 441 " + client.getNickname() +" "+ receiver + " :They aren't on that channel\r\n"
# define RPL_ENDOFNAMES ":" SERV_NAME " 366 " + client.getNickname() + " :End of /NAMES list\r\n"
# define RPL_INVITELIST ":" SERV_NAME " 346 " + receiver + " #" + channelName + ": You have been invited on " + channelName+" \r\n"
# define RPL_INVITED ":" SERV_NAME " 345 " + client.getNickname()+ " " + receiver + +  " #" + channelName +" :" + receiver +" has been invited by " + client.getNickname() + "\r\n"

/*
# define RPL_NAMREPLY			"353"
# define RPL_YOUREOPER			"381"

# define ERR_NOMOTD				"422"




# define MSG_NOSIGNAL			0x2000
# define OP_PASS				"password"
*/

# include "Channel.hpp"
# include "Client.hpp"
# include "Commande.hpp"



class Server
{
	public:

		Server();
		~Server();
		void		createServer(std::string ip, int port, std::string password);
		std::string	get_name_server() const;

		int			waitClient();
		void		addClient(int client_socket);
		void 		connect_client(int client_socket, Client & clients, std::string msg);
		void		listenClients();
		void		ClientFinder(int client_socket);

		void 		set_serv_socket(int socket);
		int 		get_serv_socket() const;

		void 		set_max_fd(int max_fd);
		int 		get_max_fd() const;

		void 		set_port(int port);
		int 		get_port() const;

		void		set_read_fds(fd_set read_fds);
		fd_set		get_read_fds() const;

		void		set_write_fds(fd_set write_fds);
		fd_set		get_write_fds() const;

		void 		set_password(std::string password);
		std::string get_password() const;

		void 		set_ip(std::string ip);
		std::string get_ip() const;

		void 		set_clients(std::list<Client> clients);
		std::list<Client> get_clients() const;

		bool		doesChannelExist(std::string name);
		bool		doesClientExist(std::string name);
		void		removeClient(Client &client);

		Client		&getClientFromNickname(std::string name);
		void    	executeCommand(Client client, std::string message);
		void		addClientToChannel(Channel &channel, Client &client);
		void		putMessage(std::string message, Client client, std::string receiver,fd_set write_fds);
		void								addChannel(Channel *channel);
		Channel								&getChannel(std::string name);
		std::map<std::string, Channel *>	getChannelMap() const;
		void								removeChannel(std::string name);

		Client      &FindClient(std::string nickname);

		void parseMessage(std::string msg, Client &client, Server &server);

		fd_set	_read_fds;
		fd_set temp_read_fds;
		fd_set temp_write_fds;
		fd_set	_write_fds;
		std::list<Client>	_clients;

	private:

		std::string 		_name_server;
		int					_server_socket;
		int					_max_fd;
		std::string			_password;
		std::string			_ip;
		int					_port;

		std::map<std::string, Channel *>	_channels;

		struct tabFunc
		{
		    typedef void (*FuncType)(std::string, Client&, Server&);
		    std::string command;
		    FuncType func;
		};
		std::map<std::string, void (*)(std::string, Client&, Server&)> commandMap;
};

#endif