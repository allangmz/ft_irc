#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <string>
# include <map>

class Channel;

class Client
{
	public:
		Client(int socket);
		~Client();
		Client &operator=(const Client &rhs);
		bool	operator==(const Client &rhs) const;

		void		set_socket(int socket);
		int			get_socket() const;

		void 		set_ip(std::string ip);
		std::string get_ip() const;

		void 		set_port(int port);
		int 		get_port() const;

		void 		set_password(std::string password);
		std::string get_password() const;

		void 		set_is_admin(bool is_admin);
		bool 		get_is_admin() const;

		void 		set_is_connected();
		void 		set_is_disconnected();
		bool 		get_is_connected() const;
		
		void 		set_buffer(std::string buffer);
		std::string get_buffer() const;

		void 		setUsername(std::string username);
		std::string getUsername() const;

		void 		setRealname(std::string realname);
		std::string getRealname() const;
	
		void 		setNickname(std::string nickname);
		std::string getNickname() const;

		void		set_status(int status);
		int			get_status() const;

		Channel *	getChannel();
		void		addChannelToClient(Channel &channel);
		void		removeChannel(Channel &channel_name);
		void		removeChannelFromClient();
		void		printMessage(std::string message);

	private:
	
		int			_socket;
		std::string	_name;
		std::string	_ip;
		int			_port;
		std::string	_password;
		bool		_is_admin;
		bool		_is_connected;

		std::string	_username;
		std::string	_realname;
		std::string	_nickname;
		

		std::string _buffer;

		Channel  * _channel;
		int			_status;

};

#endif
