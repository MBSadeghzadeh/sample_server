#include <asio.hpp>
#include <SampleMessage.pb.h>
#include <thread>
#include <chrono>

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(asio::ip::tcp::socket socket) : socket_(std::move(socket))
    {
    }

    void start()
    {
        do_write();
    }

private:
    void do_write()
    {
        auto self(shared_from_this());
        TemperatureData td;
        td.set_temperature(10.5);
        td.set_device_id("1234");
        unsigned long milliseconds_since_epoch =
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        td.set_timestamp(milliseconds_since_epoch);

        size_t length = td.ByteSizeLong();
        char *data_ = new char[8 + length];

        td.SerializeToArray(data_ + 8, length);
        memcpy(data_, &length, 8);

        asio::async_write(socket_, asio::buffer(data_, 8 + length),
                          [this, data_, length, self](asio::error_code ec, std::size_t)
                          {
                            std::cout << "Write Thread number:" << std::this_thread::get_id() << std::endl;
                              if (!ec)
                              {
                                  std::cout << "wrote:" << length  << std::endl;
                                  using namespace std::chrono_literals;
                                  std::this_thread::sleep_for(3s);
                                  delete []data_;
                                  do_write();
                              }

                          });
    }

    asio::ip::tcp::socket socket_;
};

class Server
{
public:
    Server(asio::io_context &io_context, const asio::ip::tcp::endpoint &endpoint) : acceptor_(io_context, endpoint)
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(
            [this](asio::error_code ec, asio::ip::tcp::socket socket)
            {
                std::cout << "Accept Thread number:" << std::this_thread::get_id() << std::endl;

                if (!ec)
                {
                    std::make_shared<Session>(std::move(socket))->start();
                }

                do_accept();
            });
    }
    asio::ip::tcp::acceptor acceptor_;
};

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        try
        {
            asio::io_context io_context;
            asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), std::stoi(argv[1]));
            Server server(io_context, endpoint);
            io_context.run();
        }
        catch (const std::invalid_argument &e)
        {
            std::cerr << "Please enter a valid port number." << std::endl;
        }
    }
    else
    {
        std::cout << "please enter the listening port number for the server: Server <port>" << std::endl;
    }
    return 0;
}