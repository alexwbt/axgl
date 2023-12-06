
#include <proto/net.h>
#include <spdlog/spdlog.h>

int main()
{
  flatbuffers::FlatBufferBuilder builder;
  
  auto message = net::CreateTextMessageDirect(builder, "Hello world");
  auto header = net::CreateHeader(builder, 1, 1);
}
