#include <filesystem>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>

#include <args.hxx>
#include <simdjson.h>
#include <spdlog/spdlog.h>

#include <util/file.hpp>

#define DIRECTORY_FIELD "directory"
#define COMMAND_FIELD "command"
#define FILE_FIELD "file"
#define OUTPUT_FIELD "output"

namespace fs = std::filesystem;

static std::string_view get_include_path(const std::string& file_content)
{
  const auto include = file_content.find("#include");
  if (include == std::string::npos) return {};

  const auto begin = file_content.find('<', include);
  if (begin == std::string::npos) return {};

  const auto end = file_content.find('>', begin);
  if (end == std::string::npos) return {};

  return {file_content.data() + begin + 1, end - begin - 1};
}

static void resolve_header_file(std::string& file, std::string_view command)
{
  // read cpp
  const auto file_content = util::read_text_file(file);
  const auto include_path = get_include_path(file_content);
  if (include_path.empty()) return;

  // find header path
  // only handles glued include flags
  for (auto&& subrange : command | std::views::split(' '))
  {
    const auto length = std::ranges::distance(subrange);
    if (length <= 2) continue;

    std::string_view view(&*subrange.begin(), length);
    if (!view.starts_with("-I")) continue;

    std::string_view include_dir = view.substr(2);
    const auto header_file = fs::path(include_dir) / include_path;
    if (fs::is_regular_file(header_file))
    {
      file = header_file.lexically_normal().generic_string();
      return;
    }
  }

  SPDLOG_WARN("Failed to find header file of \"{}\"", file);
}

static bool is_proxy_file(const std::string& filepath)
{
  return filepath.ends_with(".cpp")
    && filepath.find("/compile_proxy/") != std::string_view::npos
    && !filepath.ends_with("compile_proxy.cpp");
}

static int fix_compile_proxy(const std::string& directory)
{
  using namespace simdjson;

  static constexpr std::string_view kFilename = "compile_commands.json";
  const auto filepath = (fs::path(directory) / kFilename).string();
  const auto json = padded_string::load(filepath);

  // parser
  ondemand::parser parser;
  auto doc = parser.iterate(json);

  // builder
  builder::string_builder builder;
  builder.start_array();

  bool first = true;
  for (ondemand::object entry : doc)
  {
    // read
    std::string_view directory = entry[DIRECTORY_FIELD];
    std::string_view command = entry[COMMAND_FIELD];
    std::string file{std::string_view(entry[FILE_FIELD])};
    std::string_view output = entry[OUTPUT_FIELD];

    // process
    if (is_proxy_file(file)) resolve_header_file(file, command);

    // write
    if (!first) builder.append_comma();
    first = false;
    builder.append_raw("\n");
    builder.start_object();
    builder.append_raw("\n  ");
    builder.append_key_value<DIRECTORY_FIELD>(directory);
    builder.append_raw(",\n  ");
    builder.append_key_value<COMMAND_FIELD>(command);
    builder.append_raw(",\n  ");
    builder.append_key_value<FILE_FIELD>(file);
    builder.append_raw(",\n  ");
    builder.append_key_value<OUTPUT_FIELD>(output);
    builder.append_raw("\n");
    builder.end_object();
  }

  // write to file
  builder.append_raw("\n");
  builder.end_array();
  util::write_text_file(filepath, builder.view()->data());

  return 0;
}

int main(const int argc, char** argv)
{
  args::ArgumentParser parser(
    "Compilation database compile proxy post processing."
  );
  args::HelpFlag help(parser, "help", "Display the help menu.", {'h', "help"});

  args::Positional<std::string> directory(
    parser, "dir", "The directory containing compile_commands.json",
    args::Options::Required
  );

  try
  {
    parser.ParseCLI(argc, argv);
    return fix_compile_proxy(args::get(directory));
  }
  catch (const args::Completion& e)
  {
    std::cout << e.what();
    return 0;
  }
  catch (const args::Help&)
  {
    std::cout << parser;
    return 0;
  }
  catch (const std::exception& e)
  {
    SPDLOG_ERROR("{}", e.what());
    return 1;
  }
}
