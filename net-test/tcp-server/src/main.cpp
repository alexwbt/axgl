#include <chrono>
#include <format>

#include <spdlog/spdlog.h>

#include <net/tcp_server.h>


class Server : public net::TcpServer
{
  using net::TcpServer::TcpServer;

public:
  std::string get_daytime_string()
  {
    const auto now = std::chrono::system_clock::now();
    return std::format("{:%d-%m-%Y %H:%M:%OS}", now);
  }

  void on_connect(uint32_t session_id, SessionPtr session) override
  {
    SPDLOG_INFO("new connection (id: {})", session_id);

    // asio::error_code ignored_error;
    // asio::write(*socket, asio::buffer(get_daytime_string()), ignored_error);

    // remove_session(id);
  }

  void on_disconnect(uint32_t session_id) override
  {
    SPDLOG_INFO("client disconnected (id: {})", session_id);
  }

  void on_receive(uint32_t session_id, const std::string& identifier, std::vector<uint8_t> buffer) override
  {
    std::string raw_data_str(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    SPDLOG_INFO("received message from client (id: {}, type: {}): {}", session_id, identifier, raw_data_str);

    // std::string str(reinterpret_cast<const char*>(buffer.data()), size);
    // SPDLOG_INFO("Raw message. buffer_size: {}, size: {}, str: {}", buffer.size(), size, str);

    // flatbuffers::Verifier::Options verifier_options;
    // verifier_options.assert = true;

    // auto verifier = flatbuffers::Verifier(buffer.data(), size, verifier_options);
    // auto valid_message = net::VerifySizePrefixedMessageBuffer(verifier);

    // auto message = net::GetSizePrefixedMessage(buffer.data());
    // SPDLOG_INFO("Received message. valid: {}, type: {}, size: {}, subject: {}, content: {}",
    //   valid_message, type, size, message->subject()->str(), message->content()->str());
  }

};

int main()
{
  Server server(13000);
  server.start();
}
