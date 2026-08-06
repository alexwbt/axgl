#include <filesystem>
#include <regex>
#include <stdexcept>
#include <string>

#include <args.hxx>
#include <simdjson.h>
#include <spdlog/spdlog.h>

#include <util/file.hpp>

#define DIRECTORY_FIELD "directory"
#define COMMAND_FIELD "command"
#define FILE_FIELD "file"
#define OUTPUT_FIELD "output"

static bool is_proxy_file(const std::string_view& filepath)
{
  return filepath.ends_with(".cpp")
    && filepath.find("/compile_proxy/") != std::string_view::npos;
}

static void resolve_header_file(
  std::string& file, const std::string_view& command)
{
  namespace fs = std::filesystem;
  using iterator = std::regex_iterator<std::string_view::const_iterator>;

  // read cpp
  static std::regex include_pattern(R"(#include\s*<([^>]+)>)");
  const auto file_content = util::read_text_file(file);
  std::smatch match;
  const bool matched = std::regex_search(
    file_content.begin(), file_content.end(), match, include_pattern);
  if (!matched) return;

  // find header path
  static std::regex flags_pattern(R"((?:^|\s)-I\s*(\S+))");
  auto includes_begin = iterator(command.begin(), command.end(), flags_pattern);
  auto includes_end = iterator();
  for (auto i = includes_begin; i != includes_end; ++i)
  {
    const auto header_file = fs::path((*i)[1].str()) / match[1].str();
    if (fs::exists(header_file))
    {
      file = header_file.lexically_normal().generic_string();
      return;
    }
  }
}

static int fix_compile_proxy(const std::string& directory)
{
  using namespace simdjson;
  namespace fs = std::filesystem;

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
  util::write_text_file(filepath, builder.view());

  return 0;
}

int main(const int argc, char** argv)
{
  args::ArgumentParser parser(
    "Compilation database compile proxy post processing.");
  args::HelpFlag help(parser, "help", "Display the help menu.", {'h', "help"});

  args::Positional<std::string> directory(
    parser, "dir", "The directory containing compile_commands.json",
    args::Options::Required);

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
    SPDLOG_ERROR(e.what());
    return 1;
  }
}

