#include <ctime>
#include <vector>
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
static const uint32_t Port = 2001;
using namespace boost::asio;

typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;

class  talk_to_client{
public:
    explicit talk_to_client(io_service &service):_service(service){
        socket = socket_ptr(new ip::tcp::socket(_service));
        name = std::string("");
    }
    ip::tcp::socket sock(){return socket;}
private:
    io_service _service;
    socket_ptr socket;
    std::string name;
};

class MyServer{
private:
    struct _client_info{
        bool client_list_changed;
        uint32_t time_from_last_ping;
    };
    typedef struct _client_info client_info;
public:
    MyServer(){
        ip::tcp::endpoint endp( ip::tcp::v4(), Port); // listen on 2001
        ep = endp;
        ip::tcp::acceptor accept(service, ep);
        acc = accept;
    }
    ~MyServer(){
        for (uint32_t i = 0; i < Threads.size(); ++i){
            Threads[i].join();
        }
    }
    void log_init(){
        boost::log::register_simple_formatter_factory
                <boost::log::trivial::severity_level, char>("Severity");
        logging::add_file_log
                (
                        logging::keywords::file_name = "log_%N.log",
                        logging::keywords::rotation_size = SIZE_FILE,
                        logging::keywords::time_based_rotation =
                              boost::log::sinks::file::rotation_at_time_point{0,
                                                                          0, 0},
                        logging::keywords::format =
                                "[%TimeStamp%] [%Severity%] %Message%");

        logging::add_console_log(
                std::cout,
                logging::keywords::format
                        = "[%TimeStamp%] [%Severity%]: %Message%");
        logging::add_common_attributes();
    }
    void send_clients_list(socket_ptr sock){
        std::string clients_names;
        for (uint32_t i = 0; i < client_list.size(); ++i){
            clients_names += client_list[i].name + std::string(" ");
        }
        clients_names += '\n';
        sock.write_some(buffer(clients_names));
    }
    void client_session(uint32_t client_ID)
    {
        socket_ptr sock = client_info_list[client_ID]->sock();
        while (true)
        {
            std::this_thread::sleep(std::chrono::milliseconds{1});
            char data[512];
            size_t len = sock->read_some(buffer(data));
            std::string read_msg = data;
            read_msg.assign(read_msg, 0, read_msg.find('\n'));
            if (len > 0){
                if (client_list[client_ID].name == std::string("")) {
                    client_list[client_ID].name = data;
                    std::string answer = "login_ok" + '\n';
                    sock.write_some(buffer(answer));
                    BOOST_LOG_TRIVIAL(info) << "Client: '"
                                            << client_list[client_ID].name
                                            << "' successfully logged in!";
                }
                else if (data == std::string("clients")){
                    BOOST_LOG_TRIVIAL(info) << "Client: '"
                                            << client_list[client_ID].name
                                            << "' requested clients list.";
                    send_clients_list(sock);
                    client_info_list[client_ID].client_list_changed = false;
                    client_info_list[client_ID].time_from_last_ping = 0;
                }
                else if (data == std::string("ping")){
                    if (client_info_list[client_ID].client_list_changed){
                        std::string answer = "client list changed" + '\n';
                        sock.write_some(buffer(answer));
                        BOOST_LOG_TRIVIAL(info) << "Client: '"
                                     << client_list[client_ID].name
                                     << "' pinged and client list was changed";
                    }
                    else{
                        std::string answer = "ping_ok" + '\n';
                        sock.write_some(buffer(answer));
                        BOOST_LOG_TRIVIAL(info) << "Client: '"
                                                << client_list[client_ID].name
                                                << "' successfully pinged.";
                    }
                    client_info_list[client_ID].time_from_last_ping = 0;
                }
            }
        }
    }
    void start(){
        log_init();
        while (true)
        {
            auto client = std::make_shared<talk_to_client>(service);
            acceptor.accept(client->sock());
            boost::recursive_mutex::scoped_lock lock(mutex);
            client_list.push_back(client);
            client_info_list.push_back(client_info(false, 0));
            for (uint32_t i = 0; i < client_info_list.size(); ++i){
                client_info_list[i].client_list_changed = true;
            }
            Threads.push_back(boost::thread(boost::bind(client_session,
                                                     client_list.size() - 1)));
        }
    }

private:
    io_service service;
    ip::tcp::endpoint ep;
    ip::tcp::acceptor acc;
    std::vector<boost::shared_ptr<talk_to_client>> client_list;
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