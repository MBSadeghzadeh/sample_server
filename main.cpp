#include <asio.hpp>
#include <SampleMessage.pb.h>

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(asio::ip::tcp::socket socket) : socket_(std::move(socket))
    {
    }

    void start()
    {
        auto self(shared_from_this());
        do_write();
    }

private:
    void do_write()
    {
        asio::async_write(socket_, asio::buffer(data_, length),
                                 [this, self](asio::error_code ec, std::size_t )
                                 {
                                     if (!ec)
                                     {
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
                if (!ec)
                {
                    std::make_shared<Session>(std::move(socket))->start();
                }

                do_accept();
            });
    }
    asio::ip::tcp::acceptor acceptor_;
};

int main()
{

    return 0;
}