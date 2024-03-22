#include "net/session.h"

NAMESPACE_NET

/* Session */

std::shared_ptr<Session> Session::create(uint32_t id, std::shared_ptr<Socket> socket)
{
  std::shared_ptr<Session> session(new Session(id, std::move(socket)));
  // start read loop
  asio::co_spawn(session->socket_->get_executor(),
    [session]() -> asio::awaitable<void> { return session->read_buffers(); },
    asio::detached);
  // start write loop
  asio::co_spawn(session->socket_->get_executor(),
    [session]() -> asio::awaitable<void> { return session->write_buffers(); },
    asio::detached);

  return session;
}

Session::Session(uint32_t id, std::shared_ptr<Socket> socket) :
  id_(id),
  socket_(std::move(socket)),
  output_signal_(socket_->get_executor())
{
  output_signal_.expires_at(std::chrono::steady_clock::time_point::max());
}

void Session::send(DataPtr buffer)
{
  std::lock_guard<std::mutex> lock(output_queue_mutex_);
  output_queue_.push(std::move(buffer));
  output_signal_.cancel_one();
}

void Session::handle_input(std::function<void(DataPtr)> handler)
{
  std::lock_guard<std::mutex> lock(input_queue_mutex_);
  while (!input_queue_.empty())
  {
    handler(input_queue_.front());
    input_queue_.pop();
  }
}

void Session::close()
{
  socket_->close();
  output_signal_.cancel();
}

bool Session::connected()
{
  return socket_->connected();
}

asio::awaitable<void> Session::write_buffers()
{
  try
  {
    while (connected())
    {
      {
        std::lock_guard<std::mutex> lock(output_queue_mutex_);
        while (!output_queue_.empty())
        {
          co_await socket_->write_buffer(output_queue_.front());
          output_queue_.pop();
        }
      }

      asio::error_code ec;
      co_await output_signal_.async_wait(asio::redirect_error(asio::use_awaitable, ec));
    }
  }
  catch (const std::exception& e)
  {
    close();
  }
}

asio::awaitable<void> Session::read_buffers()
{
  try
  {
    while (true)
    {
      std::vector<uint8_t> buffer;
      co_await socket_->read_buffer(buffer);

      std::lock_guard<std::mutex> lock(input_queue_mutex_);
      input_queue_.push(std::make_shared<std::vector<uint8_t>>(std::move(buffer)));
    }
  }
  catch (const std::exception& e)
  {
    close();
  }
}

/* Server */

Server::Server(std::shared_ptr<asio::io_context> io_context, const asio::ip::port_type& port) :
  IoContextComponent(std::move(io_context)), port_(port)
{}

void Server::update()
{
  for (auto it = sessions_.begin(); it != sessions_.end();) {
    it->second->handle_input([this, &it](DataPtr buffer)
    {
      on_receive(it->first, std::move(buffer));
    });

    if (!it->second->connected())
    {
      on_disconnect(it->first);
      it = sessions_.erase(it);
      continue;
    }
    it++;
  }
}

void Server::send(uint32_t session_id, DataPtr buffer)
{
  if (sessions_.contains(session_id))
    sessions_.at(session_id)->send(std::move(buffer));
}

void Server::send_to_all(DataPtr buffer)
{
  for (auto& session : sessions_)
    session.second->send(buffer);
}

void Server::close_session(uint32_t session_id)
{
  if (sessions_.contains(session_id))
    sessions_.at(session_id)->close();
}

/* Client */

void Client::disconnect()
{
  if (session_)
    session_->close();
}

void Client::update()
{
  if (!session_)
    return;

  session_->handle_input([this](DataPtr buffer)
  {
    on_receive(std::move(buffer));
  });

  if (!session_->connected())
  {
    on_disconnect();
    session_ = nullptr;
  }
}

bool Client::connected()
{
  return session_ && session_->connected();
}

void Client::send(DataPtr buffer)
{
  if (session_)
    session_->send(std::move(buffer));
}

NAMESPACE_NET_END
