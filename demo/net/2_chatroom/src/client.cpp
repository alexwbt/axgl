#include <spdlog/spdlog.h>

int main()
{
#if SPDLOG_ACTIVE_LEVEL == SPDLOG_LEVEL_DEBUG
  spdlog::set_level(spdlog::level::debug);
#endif

  SPDLOG_INFO("client");
}
