
#include <fstream>
#include <filesystem>

#include <args.hxx>
#include <spdlog/spdlog.h>

#include "fbs/bundlefile.h"

struct File
{
  std::string path;
  std::string key;
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
      files.push_back({ path, key });
    }
  }
  return files;
}

int write_files(const std::vector<File>& files, const std::string& target)
{
  flatbuffers::FlatBufferBuilder builder;

  std::vector<flatbuffers::Offset<bundlefile::File>> fbs_files;
  for (const auto& file : files)
  {
    std::ifstream input_stream(file.path, std::ios::binary | std::ios::ate);
    const auto size = input_stream.tellg();
    input_stream.seekg(0, std::ios::beg);

    std::vector<unsigned char> file_data(size);
    input_stream.read(reinterpret_cast<char*>(file_data.data()), size);

    auto key_offset = builder.CreateString(file.key);
    auto data_offset = builder.CreateVector(file_data);
    bundlefile::FileBuilder file_builder(builder);
    file_builder.add_key(key_offset);
    file_builder.add_data(data_offset);
    fbs_files.push_back(file_builder.Finish());
  }

  auto fbs_files_offset = builder.CreateVector(fbs_files);
  bundlefile::BundleBuilder bundle_builder(builder);
  bundle_builder.add_files(fbs_files_offset);
  builder.Finish(bundle_builder.Finish(), bundlefile::BundleIdentifier());

  std::ofstream output_stream(target, std::ios::binary);
  if (!output_stream)
  {
    SPDLOG_ERROR("Error opening file for writing.");
    return 1;
  }

  output_stream.write(reinterpret_cast<const char*>(
    builder.GetBufferPointer()), builder.GetSize());

  return 0;
}

int bundle_files(const std::string& source, const std::string& target)
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
  return write_files(files, target);
}

int main(int argc, char** argv)
{
  args::ArgumentParser parser("Bundles all files in a directory into a binary file.");
  args::HelpFlag help(parser, "help", "Display the help menu.", { 'h', "help" });

  args::Positional<std::string> source(parser, "source", "The source directory to bundle.", args::Options::Required);
  args::Positional<std::string> target(parser, "target", "The output binary file.", args::Options::Required);

  try { parser.ParseCLI(argc, argv); }
  catch (const args::Completion& e) { std::cout << e.what(); }
  catch (const args::Help&) { std::cout << parser; }
  catch (const args::Error& e)
  {
    SPDLOG_ERROR(e.what());
    return 1;
  }

  return bundle_files(args::get(source), args::get(target));
}
