// Copyright 2020 dimakirol <your_email>

#include <header.hpp>

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
    MyServer(){
        log_init();
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
    void kicker(){
        while (true){
            std::this_thread::__sleep_for(std::chrono::seconds{0},
            std::chrono::nanoseconds{rand() % base_time + additional_time});

            if (!client_list.size()) {
                continue;
            }
            for (uint32_t i = 0; i < client_list.size(); ++i){
                if (client_info_list[i].suicide) {
                    continue;
                }
                uint32_t current_time = time(NULL);
                uint32_t difference = (current_time -
                        client_info_list[i].time_last_ping);
                if (difference > critical_time){
                    client_info_list[i].suicide = true;
                    BOOST_LOG_TRIVIAL(info) << "it must die";
                }
            }
        }
    }
    void send_clients_list(socket_ptr sock){
        std::string clients_names;

        while (!mutex_for_client_list.try_lock()) {
            unsigned now = time(0);
            std::this_thread::sleep_for(
                    std::chrono::milliseconds(rand_r(&now) % 3 + 1));
        }

        for (uint32_t i = 0; i < client_list.size(); ++i){
            clients_names += client_list[i]->name + std::string(" ");
        }

        mutex_for_client_list.unlock();

        clients_names += '\n';
        sock->write_some(assio::buffer(clients_names));
    }
    void who_is_there(uint32_t client_ID)
    {
        socket_ptr sock = client_list[client_ID]->sock();
        try {
            while (true) {
                unsigned now = time(0);
                std::this_thread::__sleep_for(std::chrono::seconds{0},
                    std::chrono::nanoseconds{
                            rand_r(&now) % base_time + additional_time});

                char data[512];
                size_t len = sock->read_some(assio::buffer(data));

                if (client_info_list[client_ID].suicide){
                    BOOST_LOG_TRIVIAL(warning) << "Killing session with: "
                                            << client_list[client_ID]->name;
                    sock->write_some(assio::buffer("too_late\n"));
                    return;
                }

                std::string read_msg = data;

                if (len > 0) {
                    if (read_msg.find('\n') != std::string::npos) {
                        read_msg.assign(read_msg, 0, read_msg.rfind('\n'));
                    } else {
                        throw std::logic_error("Received wrong message");
                    }
                } else {
                    throw std::logic_error("Received empty message");
                }

                if (client_list[client_ID]->name == std::string("")) {
                    client_list[client_ID]->name = data;

                    sock->write_some(assio::buffer("login_ok\n"));

                    BOOST_LOG_TRIVIAL(info) << "Client: "
                                            << client_list[client_ID]->name
                                            << " successfully logged in!";
                } else if (read_msg == std::string("clients")) {
                    BOOST_LOG_TRIVIAL(info) << "Client: "
                                            << client_list[client_ID]->name
                                            << " requested clients list.";

                    send_clients_list(sock);

                    client_info_list[client_ID].client_list_changed = false;
                    client_info_list[client_ID].time_last_ping = time(NULL);
                } else if (read_msg == std::string("ping")) {
                    if (client_info_list[client_ID].client_list_changed) {
                        sock->write_some(assio::buffer(
                                "client_list_changed\n"));
                        BOOST_LOG_TRIVIAL(info) << "Client:"
                                            << client_list[client_ID]->name
                             << "pinged and client list was changed";
                    } else {
                        sock->write_some(assio::buffer("ping_ok\n"));
                        BOOST_LOG_TRIVIAL(info) << "Client: "
                                                << client_list[client_ID]->name
                                                << "successfully pinged.";
                    }
                    client_info_list[client_ID].time_last_ping = time(NULL);
                } else {
                    throw std::logic_error("Received wrong message: "
                                           + read_msg);
                }
            }
        } catch (std::logic_error const& e){
            BOOST_LOG_TRIVIAL(warning) << "Received strange message";
        } catch (std::exception &e){
            if (e.what() == std::string("read_some: End of file")){
                BOOST_LOG_TRIVIAL(warning) << "This client has gone:"
                                        << client_list[client_ID]->name;
                BOOST_LOG_TRIVIAL(warning) << "Killing session with: "
                                        << client_list[client_ID]->name;
                return;
            } else {
                BOOST_LOG_TRIVIAL(warning) << e.what();
            }
        }
    }
    void start(){
        assio::ip::tcp::endpoint ep(ip::tcp::v4(), Port); // listen on 2001
        assio::ip::tcp::acceptor acc(service, ep);

         Threads.push_back(boost::thread(boost::bind(&MyServer::kicker,
                 this)));
        while (true){
            auto client = std::make_shared<talk_to_client>(service);
            acc.accept(*(client->sock()));

            while (!mutex_for_client_list.try_lock()) {
                unsigned now = time(0);
                std::this_thread::sleep_for(
                        std::chrono::milliseconds(rand_r(&now) % 3 + 1));
            }
            client_list.push_back(client);
            mutex_for_client_list.unlock();

            client_info new_client;
            new_client.client_list_changed = false;
            new_client.time_last_ping = time(NULL);
            new_client.suicide = false;

            client_info_list.push_back(new_client);
            for (uint32_t i = 0; i < client_info_list.size() - 1; ++i){
                client_info_list[i].client_list_changed = true;
            }

            Threads.push_back(boost::thread(
                    boost::bind(&MyServer::who_is_there, this,
                                                    client_list.size() - 1)));
        }
    }

private:
    assio::io_service service;
    std::mutex mutex_for_client_list;
    std::vector<std::shared_ptr<talk_to_client>> client_list;
    std::vector<client_info> client_info_list;
    std::vector<boost::thread> Threads;
};

int main()
{
    try {
        MyServer server;
        server.start();
    } catch (std::exception const& e) {
        BOOST_LOG_TRIVIAL(fatal) << e.what();
    }
    return 0;
}
