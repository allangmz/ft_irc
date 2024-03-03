#include "Server.hpp"

Client::Client(int client_socket) : _socket(client_socket), _name(""), _ip(""), _port(0), _password(""), _username(""), _realname(""), _nickname(""), _buffer(""), _channel(NULL), _status(0)
{
	this->_is_admin = false;
	this->_is_connected = false;
}

Client::~Client()
{
}

Client &Client::operator=(const Client &rhs)
{
	if (this != &rhs)
	{
		this->_socket = rhs._socket;
		this->_name = rhs._name;
		this->_password = rhs._password;
		this->_ip = rhs._ip;
		this->_port = rhs._port;
		this->_is_admin = rhs._is_admin;
		this->_is_connected = rhs._is_connected;
		this->_buffer = rhs._buffer;
		this->_username = rhs._username;
		this->_nickname = rhs._nickname;
		this->_channel = rhs._channel;
		this->_status = rhs._status;
	}
	return (*this);
}

bool Client::operator==(const Client &rhs) const
{
	if (this->_nickname == rhs.getNickname())
		return (true);
	return (false);
}

void	Client::set_socket(int socket){
	_socket = socket;
}

int		Client::get_socket() const{
	return (_socket);
}

void	Client::set_ip(std::string ip){
	_ip = ip;
}

std::string	Client::get_ip() const{
	return (_ip);
}

void	Client::set_port(int port){
	_port = port;
}

int		Client::get_port() const{
	return (_port);
}

void	Client::set_password(std::string password){
	_password = password;
}

std::string	Client::get_password() const{
	return (_password);
}


void	Client::set_is_admin(bool is_admin){
	_is_admin = is_admin;
}

bool	Client::get_is_admin() const{
	return (_is_admin);
}

void	Client::set_is_connected(){
	if (_username != "" && _nickname != "" && _password != "" && get_status() == 3)
		_is_connected = true;
}

void	Client::set_is_disconnected(){
	_is_connected = false;
}

bool	Client::get_is_connected() const{
	return (_is_connected);
}

void	Client::set_buffer(std::string buffer){
	this->_buffer = buffer;
}

std::string	Client::get_buffer() const{
	return (this->_buffer);
}



Channel * Client::getChannel()
{
	return (this->_channel);
}

void Client::addChannelToClient(Channel &channel)
{
	this->_channel = &channel;
}

void	Client::removeChannel(Channel & channel_name)///KESAKO
{
	if (this->_channel == &channel_name)
		this->_channel = NULL;
}

void	Client::removeChannelFromClient()
{
	this->_channel = NULL;
}

void	Client::printMessage(std::string message)
{
	send(this->get_socket(), message.c_str(), message.length(), 0);
}

void	Client::setNickname(std::string nickname)
{
	this->_nickname = nickname;
}

std::string	Client::getNickname() const
{
	return (this->_nickname);
}

void	Client::setRealname(std::string realname)
{
	this->_realname = realname;
}

std::string	Client::getRealname() const
{
	return (this->_realname);
}

void	Client::setUsername(std::string username)
{
	this->_username = username;
}

std::string	Client::getUsername() const
{
	return (this->_username);
}

void		Client::set_status(int status)
{
	this->_status = status;
}

int			Client::get_status() const
{
	return (this->_status);
}