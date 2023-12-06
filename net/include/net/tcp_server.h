#pragma once

#include <unordered_map>
#include <memory>
#include <queue>

#include "net/asio.h"

namespace net
{

  class TcpServer
  {
  public:
    class Session final
    {
    private:
      TcpServer& server_;
      uint32_t id_;
      asio::ip::tcp::socket socket_;
      asio::steady_timer timer_;
      std::queue<std::string> output_queue_;

    public:
      Session(TcpServer& server, uint32_t id, asio::ip::tcp::socket socket);
      ~Session();

      void send_message(const std::string& message);

    private:
      asio::awaitable<void> write_messages();
      asio::awaitable<void> read_messages();
    };

    typedef std::shared_ptr<Session> SessionPtr;

  private:
    const asio::ip::port_type port_;
    asio::io_context io_context_;
    asio::ip::tcp::acceptor acceptor_;
    std::unordered_map<uint32_t, SessionPtr> sessions_;

    uint32_t next_session_id_ = 0;

  public:
    TcpServer(asio::ip::port_type port);
    virtual ~TcpServer() {}

    void start();
    void stop();

    void send_message(uint32_t session_id);
    void remove_session(uint32_t session_id);

    virtual void on_connect(uint32_t session_id, SessionPtr session) = 0;
    virtual void on_disconnect(uint32_t session_id) = 0;
    virtual void on_receive(uint32_t session_id, const std::string& message) = 0;

  private:
    uint32_t use_next_session_id();
    asio::awaitable<void> accept_connections();
  };

}
