#include <format>
#include <vector>
#include <string>
#include <stdint.h>
#include <fstream>
#include <iostream>
#include <filesystem>

#include <spdlog/spdlog.h>
#include <args.hxx>

struct File
{
  std::string path;
  std::string key;
  size_t key_hash;
};

inline std::string entry_to_string(const std::filesystem::path& path)
{
  std::stringstream stream;
  for (const auto& e : path)
    stream << e.string() << "/";
  auto value = stream.str();
  value.pop_back();
  return value;
}

std::vector<File> read_directory(const std::string& source)
{
  std::vector<File> files;
  std::hash<std::string> to_hash;
  for (const auto& entry : std::filesystem::recursive_directory_iterator(source))
  {
    if (!std::filesystem::is_directory(entry.path()))
    {
      auto path = entry_to_string(entry.path());
      auto key = entry_to_string(std::filesystem::relative(entry.path(), source));
      auto key_hash = to_hash(key);
      files.push_back({ path, key, key_hash });
    }
  }
  return files;
}

void write_files(const std::vector<File>& files, const std::string& target, const std::string& ns)
{
  std::ofstream header_output_stream(target + ".hpp");
  header_output_stream << "#pragma once" << std::endl;
  header_output_stream << "#include <span>" << std::endl;
  header_output_stream << "#include <array>" << std::endl;
  header_output_stream << "#include <string>" << std::endl;
  header_output_stream << "#include <stdint.h>" << std::endl;
  header_output_stream << "#include <unordered_map>" << std::endl;
  if (!ns.empty()) header_output_stream << "namespace " << ns << " {" << std::endl;
  header_output_stream << "extern const std::unordered_map<std::string, std::span<const uint8_t>> data;" << std::endl;
  header_output_stream << "inline std::string get(const std::string& key) { return std::string(data.at(key).begin(), data.at(key).end()); }" << std::endl;
  if (!ns.empty()) header_output_stream << "}" << std::endl;

  std::ofstream output_stream(target + ".cpp");
  output_stream << "#include " << std::filesystem::path(target + ".hpp").filename() << std::endl;
  if (!ns.empty()) output_stream << "namespace " << ns << " {" << std::endl;

  for (const auto& file : files)
  {
    std::ifstream input_stream(file.path, std::ios::binary | std::ios::ate);
    const auto size = input_stream.tellg();
    input_stream.seekg(0, std::ios::beg);

    std::vector<char> buffer;
    buffer.resize(size);
    input_stream.read(buffer.data(), size);

    output_stream << std::format("const std::array<uint8_t, {}> d{} = {{", buffer.size(), file.key_hash);
    for (char b : buffer)
      output_stream << static_cast<int>(b) << ",";
    output_stream << "};" << std::endl;
  }

  output_stream << "const std::unordered_map<std::string, std::span<const uint8_t>> data = {" << std::endl;
  for (const auto& file : files)
    output_stream << std::format("{{\"{}\", d{}}},", file.key, file.key_hash) << std::endl;
  output_stream << "};" << std::endl;
  if (!ns.empty()) output_stream << "}" << std::endl;
}

int embed_files(const std::string& source, const std::string& target, const std::string& ns)
{
  if (!std::filesystem::is_directory(source))
  {
    SPDLOG_ERROR("Source must be a directory.");
    return 1;
  }
  if (std::filesystem::is_directory(target))
  {
    SPDLOG_ERROR("Target must not be a directory.");
    return 1;
  }

  const auto files = read_directory(source);
  write_files(files, target, ns);

  return 0;
}

int main(int argc, char** argv)
{
  args::ArgumentParser parser("Embeds all files in a directory into a cpp file.");
  args::HelpFlag help(parser, "help", "Display the help menu.", { 'h', "help" });
  args::CompletionFlag completion(parser, { "complete" });

  args::Positional<std::string> source(parser, "source", "The source directory to embed.", args::Options::Required);
  args::Positional<std::string> target(parser, "target", "The output cpp file.", args::Options::Required);
  args::ValueFlag<std::string> ns(parser, "namespace", "Optional namespace.", { 'n', "namespace" });

  try { parser.ParseCLI(argc, argv); }
  catch (const args::Completion& e) { std::cout << e.what(); }
  catch (const args::Help&) { std::cout << parser; }
  catch (const args::Error& e)
  {
    SPDLOG_ERROR(e.what());
    return 1;
  }

  return embed_files(args::get(source), args::get(target), args::get(ns));
}
