#include "Server.hpp"

/* bonjour thomas */

Server::Server()
{
	this->_max_fd = 0;
	FD_ZERO(&this->_read_fds);
	FD_ZERO(&this->_write_fds);

	struct tabFunc
	{
		typedef void (*FuncType)(std::string, Client&, Server&);
		std::string command;
		FuncType func;
	};

	this->commandMap["CAP"] = &connect_cap;	
	this->commandMap["NICK"] = &connect_nick;
	this->commandMap["PASS"] = &connect_pass;
	this->commandMap["PING"] = &connect_ping;

	this->commandMap["USER"] = &user; //
	this->commandMap["TOPIC"] = &topic; //
	this->commandMap["JOIN"] = &join; //
	this->commandMap["PART"] = &part; //
	this->commandMap["PRIVMSG"] = &privmsg; //
	this->commandMap["NAMES"] = &names; //
	this->commandMap["LIST"] = &list; //
	this->commandMap["QUIT"] = &quit; //
	
	/*
	commandes admin
	*/
	this->commandMap["KICK"] = &kick;
	this->commandMap["INVITE"] = &invite;
	this->commandMap["MODE"] = &mode;
}

Server::~Server()
{
	std::list<Client>::const_iterator ite = this->_clients.end();
	for (std::list<Client>::const_iterator it = this->_clients.begin(); it != ite; it++)
	{
		close(it->get_socket());
		// delete &(*it);
	}	
}

void Server::createServer(std::string ip, int port, std::string password)
{
	this->_ip = ip;
	this->_port = port;
	this->_password = password;
	this->_name_server = "IRC_serv";
	struct sockaddr_in server_addr;
	std::map<std::string,Client&>	_operators;
	int sockfd;
	int opt = 1;
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		std::cerr << "Error: socket creation failed" << std::endl;
		exit(-1);
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1)
	{
		std::cerr << "Error when setting socket options." << std::endl;
		close(sockfd);
		exit(-1);
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(this->_port);

	inet_aton(this->_ip.c_str(), (struct in_addr *) &server_addr.sin_addr.s_addr);

	if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
	{
		std::cerr << "Error: bind failed" << std::endl;
		exit(-1);
	}

	if (listen(sockfd, 10) == -1)
	{
		std::cerr << "Error: listen failed" << std::endl;
		exit(-1);
	}

	fcntl(sockfd, F_SETFL, O_NONBLOCK);
	this->_server_socket = sockfd;
	this->_max_fd = sockfd;
	
	FD_SET(this->_server_socket, &this->_read_fds);
	FD_SET(this->_server_socket, &this->_write_fds);
}


int Server::waitClient()
{
	if (FD_ISSET(this->_server_socket,&this->temp_read_fds))
	{
		FD_CLR(this->_server_socket, &this->temp_read_fds);
		return (accept(this->_server_socket, NULL, NULL));
	}
	return (-1);
}

void Server::addClient(int client_socket)
{
	Client new_client(client_socket);
	_clients.push_back(new_client);
	fcntl(client_socket, F_SETFL, O_NONBLOCK);
	FD_SET(client_socket, &this->_read_fds);
	FD_SET(client_socket, &this->_write_fds);
	if (client_socket > this->_max_fd)
		this->_max_fd = client_socket;

	std::cout << "New client connected" << std::endl;
}

void Server::listenClients()
{
	char buffer[512];
	for (std::list<Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		int fd = (*it).get_socket();
		if (FD_ISSET(fd, &this->temp_read_fds))
		{
			memset(buffer, 0, 512);
			int res = recv(fd, buffer, sizeof(buffer), MSG_DONTWAIT);
			if (res == 0)
			{
				close(it->get_socket());
				FD_CLR(it->get_socket(), &this->_read_fds);
				this->_clients.erase(it);
			}
			if (res > 0)
			{
				std::string msg = buffer;
				parseMessage(msg, *it, *this);
				msg = "";
				return;	
			}
			FD_CLR(fd, &this->temp_read_fds);
		}	
	}
}

std::string Server::get_name_server() const {
	return this->_name_server;
}

void	Server::set_serv_socket(int socket){
	this->_server_socket = socket;
}

int		Server::get_serv_socket() const{
	return (this->_server_socket);
}

void	Server::set_max_fd(int max_fd){
	this->_max_fd = max_fd;
}

int		Server::get_max_fd() const{
	return (this->_max_fd);
}

void	Server::set_port(int port){
	this->_port = port;
}

int		Server::get_port() const{
	return (this->_port);
}

void	Server::set_read_fds(fd_set read_fds){
	this->_read_fds = read_fds;
}

fd_set Server::get_read_fds() const {
	return (this->_read_fds);
}

void	Server::set_write_fds(fd_set write_fds){
	this->_write_fds = write_fds;
}

fd_set Server::get_write_fds() const {
	return (this->_write_fds);
}

void	Server::set_password(std::string password){
	this->_password = password;
}

std::string	Server::get_password() const{
	return (this->_password);
}

void	Server::set_ip(std::string ip){
	this->_ip = ip;
}

std::string	Server::get_ip() const{
	return (this->_ip);
}

void	Server::set_clients(std::list<Client> clients){
	_clients = clients;
}

std::list<Client>	Server::get_clients() const{
	return (_clients);
}

bool	Server::doesChannelExist(std::string name)
{
	std::map<std::string, Channel *>::const_iterator	it = this->_channels.begin();
	std::map<std::string, Channel *>::const_iterator	ite = this->_channels.end();
	for (; it != ite; it++)
	{
		if (it->first == name)
		{	
	
			return (true);
		}
	}
	return (false);
}

void	Server::addClientToChannel(Channel &channel, Client &client)
{
	channel.addClient(client);
	client.addChannelToClient(channel);
}

void	Server::addChannel(Channel *channel)
{
	this->_channels.insert(std::pair<std::string, Channel*>(channel->getName(), channel));
}

Channel	&Server::getChannel(std::string name)
{
	if (doesChannelExist(name) == false)
		std::cout << "Channel not found BOUFFON" << std::endl;
	return (*this->_channels[name]);
}

std::map<std::string, Channel*>	Server::getChannelMap() const
{
	return (this->_channels);
}

bool		Server::doesClientExist(std::string name)
{
	std::list<Client>::const_iterator	it = this->_clients.begin();
	std::list<Client>::const_iterator	ite = this->_clients.end();
	for (; it != ite; it++)
	{
		if (it->getNickname() == name)
			return (true);
	}
	return (false);
}

Client		&Server::getClientFromNickname(std::string name)
{
	std::list<Client>::iterator	it = this->_clients.begin();
	for (; it != this->_clients.end() && it->getNickname() != name; it++)
	{
		if (it->getNickname() == name)
			return (*it);
	}
	std::cout << "Client not found" << std::endl;
	return (*it);
}

void	Server::removeClient(Client &client)
{
	this->_clients.remove(client);
}

void	Server::removeChannel(std::string name)
{
	std::cout<< "Channel " << name << " removed" << std::endl;
	this->_channels.erase(name);
	return ;
}

Client	&Server::FindClient(std::string nickname)
{
	std::list<Client>::iterator	it = this->_clients.begin();
	std::list<Client>::iterator	ite = this->_clients.end();
	for (; it != ite; it++)
	{
		if (it->getNickname() == nickname)
			return (*it);
	}
	std::cout << "Client not found" << std::endl;
	return (*it);
}

void	Server::putMessage(std::string message, Client client, std::string receiver,fd_set write_fds)
{
	//send message to all clients in channel
	for (std::list<Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if (it->getNickname() != client.getNickname())
		{
			if (FD_ISSET(it->get_socket(), &write_fds))
				it->printMessage(":" + client.getNickname() + " PRIVMSG " + receiver + " :" + message + " \r\n");
		}
	}
}