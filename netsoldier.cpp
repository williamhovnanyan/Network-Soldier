//Yeng nervaynaca sencem anum vopshem)))

#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/thread.hpp>
#include <algorithm>
#include <vector>
#include <deque>
#include <string>
using namespace std;
using namespace boost::asio;
using namespace boost::asio::ip;




class Serializable
{
public:
	//ovveride and return vector of strings which should be sent
	//any member of this vector must start and end with ':' symbol
	virtual vector<string> serialize()=0;	

	//override and change the type of returning value to deserialized class type
	virtual void deserialize(vector<string>)=0;		
};

class tcp_client
{
	static boost::asio::io_service my_io_service;
	static tcp::socket* socket;
	static string m_host;
	static string m_port;
	static vector<string> m_Data;
public:
	tcp_client(string host = "localhost", string port="2222")
	{
		socket= new tcp::socket(my_io_service);
		m_host=host;
		m_port=port;
	}
	static void start_client()
	{
		try
		{
			//boost::this_thread::sleep(boost::posix_time::seconds(2));
			tcp::resolver resolver(my_io_service);
			tcp::resolver::query query(m_host, m_port);
			tcp::resolver::iterator endpoint_iterator =
				resolver.resolve(query);
			tcp::resolver::iterator end;
			//tcp::socket socket(my_io_service);
			boost::system::error_code error= boost::asio::error::host_not_found;
			while (error && endpoint_iterator!=end)
			{
				socket->close();
				socket->connect(*endpoint_iterator++, error);
			}
			if(error)
				throw boost::system::system_error(error);
		}
		catch(std::exception& e)
		{
			std::cerr<<e.what()<<std::endl;
		}
	}
	vector<string>& getData()
	{
		return m_Data;
	}
	static void read()
	{
		try 
		{
			{
				boost::array<char, 128> buf;
				boost::system::error_code error;

				size_t len=socket->read_some(boost::asio::buffer(buf), error);
				if(error == boost::asio::error::eof)
					return;
				else if(error)
					throw boost::system::system_error(error);

				buf.data()[len]=0;
				fillvector(buf.data(),len);
			}		
				
		}
		catch(std::exception& e)
		{
			std::cerr<<e.what()<<endl;
		}
	}
private:
	static void fillvector(char* buf, size_t len)
	{
		int i=0,startidx,endidx;
		bool f=false;
		while (i<len)
		{
			if(buf[i]==':' && !f)
			{
				startidx=i;
				f=true;
				i++;
			}
			else if(buf[i]==':' && f)
				 {
				 	endidx=i;
					f=false;
					m_Data.push_back(string(buf+startidx, buf+endidx+1));
					i++;
				  }
			else i++;
		}
	}

};

string tcp_client::m_port;
string tcp_client::m_host;
tcp::socket* tcp_client::socket;
boost::asio::io_service tcp_client::my_io_service;
vector<string> tcp_client::m_Data;


class tcp_server
{
	vector<string> m_Data;
	tcp::acceptor* acceptor;
	tcp::socket* socket;
	boost::asio::io_service my_io_service;
public: 
	tcp_server(string port="2222")
	{
		try
		{
			acceptor= new tcp::acceptor(my_io_service, 
				tcp::endpoint(tcp::v4(), atoi(port.data())));
			socket= new tcp::socket(my_io_service);
			(*acceptor).accept(*socket);
		}
		catch(std::exception& e)
		{
			std::cerr<<e.what()<<endl;
		}
	}
	void setData(vector<string>& data)
	{
		m_Data=data;
	}
	void send()
	{
		try
		{
			for(int i=0;i<m_Data.size();++i)
			{
				string message = m_Data[i];
				boost::system::error_code ignored_error;
				boost::asio::write(*socket, boost::asio::buffer(message),
					boost::asio::transfer_all(), ignored_error);
			}
		}
		catch(exception& e)
		{
			cerr<<e.what()<<endl;
		}
	}
};

class Messanger
{
	tcp_server* server;
	tcp_client* client;
	vector<string> deserializable;
	vector<string> serialized;
public:
	Messanger(string host="46.19.96.114", string port="2222")
	{
		client= new tcp_client(host);
		boost::thread client_thread(&tcp_client::start_client);
		server= new tcp_server;
	}
	void sendObject(Serializable& obj)
	{
		//serialized= obj.serialize();
		serialized.push_back(":Barev:");
		serialized.push_back(":vonces?:");
		serialized.push_back(":inch ka chka:");
		server->setData(serialized);		//sending serialized to host
		server->send();
	}
	void reciveObject(Serializable& obj)
	{
		client->read();					//recive data in deserializable
		deserializable = client->getData();
		for(int i=0;i<deserializable.size();i++)
			cout<<deserializable[i];
		//obj.deserialize(deserializable);
	}
};
class myclass:public Serializable
{
	virtual vector<string> serialize()
	{
		return vector<string>();
	}
	virtual void deserialize(vector<string>)
	{	}
};

int main()
{
	
	try
	{
		myclass mobj;
		Messanger msg;
		msg.sendObject(mobj);
		msg.reciveObject(mobj);
	}
	catch(exception& e)
	{
		cerr<<e.what()<<endl;
	}
	//client_thread.start_thread();
	

	//tcp_client client("10.133.53.195");
	
	
	return 0;
}
