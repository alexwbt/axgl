#pragma once

#include <unordered_map>
#include <memory>
#include <queue>

#include <flatbuffers/flatbuffers.h>

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
      std::queue<flatbuffers::DetachedBuffer> output_queue_;

    public:
      Session(TcpServer& server, uint32_t id, asio::ip::tcp::socket socket);
      ~Session();

      void send(flatbuffers::DetachedBuffer buffer);

    private:
      asio::awaitable<void> write_buffers();
      asio::awaitable<void> read_buffers();
    };

    typedef std::shared_ptr<Session> SessionPtr;

  private:
    const asio::ip::port_type port_;
    asio::io_context io_context_;
    asio::ip::tcp::acceptor acceptor_;
    std::unordered_map<uint32_t, SessionPtr> sessions_;

    uint32_t next_session_id_ = 1;

  public:
    TcpServer(asio::ip::port_type port);
    virtual ~TcpServer() {}

    void start();
    void stop();

    void send(uint32_t session_id, flatbuffers::DetachedBuffer buffer);
    void remove_session(uint32_t session_id);

    virtual void on_connect(uint32_t session_id, SessionPtr session) = 0;
    virtual void on_disconnect(uint32_t session_id) = 0;
    virtual void on_receive(uint32_t session_id, const std::string& identifier, std::vector<uint8_t> buffer) = 0;

  private:
    uint32_t use_next_session_id();
    asio::awaitable<void> accept_connections();
  };

}
