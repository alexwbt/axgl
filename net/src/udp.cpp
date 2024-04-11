#include "net/udp.h"

NAMESPACE_NET

UdpServer::UdpServer(std::shared_ptr<asio::io_context> io_context, const asio::ip::port_type& port) :
  Server(io_context, port),
  socket_(*io_context, { asio::ip::udp::v4(), port })
{}

NAMESPACE_NET_END
