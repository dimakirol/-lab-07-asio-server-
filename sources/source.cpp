#include <ctime>
#include <vector>
#include <thread>
#include <cstdint>
#include <iostream>
#include <string>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/log/expressions/keyword.hpp>

static const uint32_t SIZE_FILE = 10*1024*1024;
static const uint32_t Port = 2001;

using namespace boost::asio;
using std::exception;
namespace logging = boost::log;

typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;

class  talk_to_client{
public:
    explicit talk_to_client(io_service &service){
        socket = socket_ptr(new ip::tcp::socket(service));
        name = std::string("");
    }
    socket_ptr sock(){return socket;}
private:
    socket_ptr socket;

public:
    std::string name;
};

class MyServer{
private:
    struct _client_info{
        bool client_list_changed;
        uint32_t time_last_ping;
        bool suicide;
    };
    typedef struct _client_info client_info;
public:
    MyServer(){}
    ~MyServer(){
        for (uint32_t i = 0; i < Threads.size(); ++i){
            Threads[i].join();
        }
    }
    void log_init(){
        boost::log::register_simple_formatter_factory
                <boost::log::trivial::severity_level, char>("Severity");
        /*logging::add_file_log
                (
                        logging::keywords::file_name = "log_%N.log",
                        logging::keywords::rotation_size = SIZE_FILE,
                        logging::keywords::time_based_rotation =
                              boost::log::sinks::file::rotation_at_time_point{0,
                                                                          0, 0},
                        logging::keywords::format =
                                "[%TimeStamp%] [%Severity%] %Message%");

        */logging::add_console_log(
                std::cout,
                logging::keywords::format
                        = "[%TimeStamp%] [%Severity%]: %Message%");
        logging::add_common_attributes();
    }
    void send_clients_list(socket_ptr sock){
        std::string clients_names;

        std::mutex back_door;
        back_door.lock();
        for (uint32_t i = 0; i < client_list.size(); ++i){
            clients_names += client_list[i]->name + std::string(" ");
        }
        back_door.unlock();
        clients_names += '\n';
        sock->write_some(buffer(clients_names));
    }
    void kicker(){
        while (true){
            std::this_thread::__sleep_for(std::chrono::seconds{1},
                                          std::chrono::nanoseconds{0});
            std::mutex back_door;
            back_door.lock();
	        if (!client_list.size())
		     continue;
	        for (uint32_t i = 0; i < client_list.size(); ++i){
                uint32_t time = clock();
                uint32_t t = (time -
                        client_info_list[i].time_last_ping) / CLOCKS_PER_SEC;
                BOOST_LOG_TRIVIAL(info) << "difference = "
                                        << t;
                if (t > 5){
                    client_info_list[i].suicide = true;
                    BOOST_LOG_TRIVIAL(info) << "it must die";
                }
            }
            back_door.unlock();
        }
    }
    void who_is_there(uint32_t client_ID)
    {
        std::mutex door;
        door.lock();
        socket_ptr sock = client_list[client_ID]->sock();
        door.unlock();
        try {
            while (true) {
                char data[512];
                size_t len = sock->read_some(buffer(data));

                BOOST_LOG_TRIVIAL(info) << "it must die = " << client_info_list[client_ID].suicide;
                if (client_info_list[client_ID].suicide){
                    BOOST_LOG_TRIVIAL(info) << "Killing session with: "
                                            << client_list[client_ID]->name;
                    client_list.erase(client_list.begin() + client_ID);
                    client_info_list.erase(client_info_list.begin() + client_ID);
                    return;
                }

                std::string read_msg = data;
                BOOST_LOG_TRIVIAL(info) << "New message: "
                                        << read_msg;
                if (read_msg.find('\n') != std::string::npos)
                     read_msg.assign(read_msg, 0, read_msg.rfind('\n'));
                if (len > 0) {
                    if (client_list[client_ID]->name == std::string("")) {
                        client_list[client_ID]->name = data;
                        std::string answer = std::string("login_ok");
                        answer += '\n';
                        sock->write_some(buffer(answer));
                        //BOOST_LOG_TRIVIAL(info) << "Client: "
                        //                        << client_list[client_ID]->name
                        //                        << " successfully logged in!";
                    } else if (read_msg.find("clients") != std::string::npos) {
                        //BOOST_LOG_TRIVIAL(info) << "Client: "
                        //                        << client_list[client_ID]->name
                        //                        << " requested clients list.";
                        send_clients_list(sock);
                        client_info_list[client_ID].client_list_changed = false;
                        client_info_list[client_ID].time_last_ping = clock();
                    } else if (read_msg.find("ping") != std::string::npos) {
                        if (client_info_list[client_ID].client_list_changed) {
                            std::string answer = std::string("client_list_changed");
                            answer += '\n';
                            sock->write_some(buffer(answer));
//                        BOOST_LOG_TRIVIAL(info) << "Client:"
//                                                << client_list[client_ID]->name
//                                                << "pinged and client list was changed";
                        } else {
                            std::string answer = std::string("ping_ok");
                            answer += '\n';
                            sock->write_some(buffer(answer));
//                            BOOST_LOG_TRIVIAL(info) << "Client: "
//                                                    << client_list[client_ID]->name
//                                                    << "successfully pinged.";
                        }
                        client_info_list[client_ID].time_last_ping = clock();
                    }
                }
            }
        }
        catch(exception &e){
            BOOST_LOG_TRIVIAL(info) << e.what();
            if (e.what() == std::string("read_some: End of file")){
                BOOST_LOG_TRIVIAL(info) << "This client has gone:"
                                        << client_list[client_ID]->name;
                BOOST_LOG_TRIVIAL(info) << "Killing session with: "
                                        << client_list[client_ID]->name;
                client_list.erase(client_list.begin() + client_ID);
                client_info_list.erase(client_info_list.begin() + client_ID);
                return;
            }
        }
    }
    void start(){
        log_init();
        ip::tcp::endpoint ep(ip::tcp::v4(), Port); // listen on 2001
        ip::tcp::acceptor acc(service, ep);

	Threads.push_back(boost::thread(boost::bind(&MyServer::kicker, this)));
        while (true)
        {
            auto client = std::make_shared<talk_to_client>(service);
            acc.accept(*(client->sock()));
            mutex.lock();
            client_list.push_back(client);

            client_info new_client;
            new_client.client_list_changed = false;
            new_client.time_last_ping = clock();
            new_client.suicide = false;
            client_info_list.push_back(new_client);

            for (uint32_t i = 0; i < client_info_list.size() - 1; ++i){
                //S P E C I A L  F O R  D I M O N!)
                client_info_list[i].client_list_changed = true;
            }
            Threads.push_back(boost::thread(boost::bind(&MyServer::who_is_there, this,
                                                     client_list.size() - 1)));
            mutex.unlock();
        }
    }

private:
    io_service service;
    std::vector<std::shared_ptr<talk_to_client>> client_list;
    std::vector<client_info> client_info_list;
    std::mutex mutex;
    std::vector<boost::thread> Threads;
};

int main()
{
    MyServer server;
    server.start();
    return 0;
}