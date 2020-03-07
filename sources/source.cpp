#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/log/trivial.hpp>

//using boost::asio::ip::tcp;
using namespace boost::asio;

//Here is a simple synchronous server:using boost::asio;
typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;

void client_session(socket_ptr sock)
{
    while ( true)
    {
        char data[512];
        size_t len = sock->read_some(buffer(data));
        if ( len > 0)
            BOOST_LOG_TRIVIAL(info) << data;
    }
}

int main()
{
    io_service service;
    ip::tcp::endpoint ep( ip::tcp::v4(), 2001); // listen on 2001
    ip::tcp::acceptor acc(service, ep);
    while (true)
    {
        socket_ptr sock(new ip::tcp::socket(service));
        acc.accept(*sock);
        boost::thread(boost::bind(client_session, sock));
    }

    return 0;
}