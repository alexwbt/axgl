#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <bundlefile/bundle_fbs.h>
#include <bundlefile/unbundle.hpp>

namespace {

std::filesystem::path write_test_bundle(
  const std::vector<std::pair<std::string, std::vector<uint8_t>>>& entries
) {
  flatbuffers::FlatBufferBuilder builder;

  std::vector<flatbuffers::Offset<fbs::bundlefile::File>> fbs_files;
  for (const auto& [key, data] : entries) {
    auto key_off = builder.CreateString(key);
    auto data_off = builder.CreateVector(data);
    fbs::bundlefile::FileBuilder fb(builder);
    fb.add_key(key_off);
    fb.add_data(data_off);
    fbs_files.push_back(fb.Finish());
  }

  auto files_off = builder.CreateVector(fbs_files);
  fbs::bundlefile::BundleBuilder bb(builder);
  bb.add_files(files_off);
  builder.Finish(bb.Finish(), fbs::bundlefile::BundleIdentifier());

  const auto path
    = std::filesystem::temp_directory_path() / "axgl_bundle_test.bnd";
  std::ofstream out(path, std::ios::binary);
  out.write(
    reinterpret_cast<const char*>(builder.GetBufferPointer()),
    static_cast<std::streamsize>(builder.GetSize())
  );
  return path;
}

} // namespace

TEST_SUITE("bundlefile round-trip") {
  TEST_CASE("empty bundle is readable") {
    const auto path = write_test_bundle({});
    const bundlefile::Bundle bundle(path.string());
    const auto* root = bundle.get_bundle();
    REQUIRE(root != nullptr);
    CHECK(root->files()->size() == 0);
    std::filesystem::remove(path);
  }

  TEST_CASE("single entry round-trips key and data") {
    const std::vector<uint8_t> data{'h', 'e', 'l', 'l', 'o'};
    const auto path = write_test_bundle({{"a.txt", data}});
    const bundlefile::Bundle bundle(path.string());
    const auto* root = bundle.get_bundle();
    REQUIRE(root != nullptr);
    REQUIRE(root->files() != nullptr);
    REQUIRE(root->files()->size() == 1);

    const auto* file = root->files()->Get(0);
    CHECK(file->key()->str() == "a.txt");
    REQUIRE(file->data() != nullptr);
    REQUIRE(file->data()->size() == data.size());
    CHECK(std::equal(data.begin(), data.end(), file->data()->begin()));
    std::filesystem::remove(path);
  }

  TEST_CASE("multiple entries preserve order") {
    const std::vector<uint8_t> d1{1, 2, 3};
    const std::vector<uint8_t> d2{4, 5};
    const std::vector<uint8_t> d3{6};
    const auto path
      = write_test_bundle({{"first", d1}, {"second", d2}, {"third", d3}});
    const bundlefile::Bundle bundle(path.string());
    const auto* root = bundle.get_bundle();
    REQUIRE(root->files() != nullptr);
    REQUIRE(root->files()->size() == 3);
    CHECK(root->files()->Get(0)->key()->str() == "first");
    CHECK(root->files()->Get(1)->key()->str() == "second");
    CHECK(root->files()->Get(2)->key()->str() == "third");
    CHECK(root->files()->Get(0)->data()->size() == 3);
    std::filesystem::remove(path);
  }

  TEST_CASE("binary data round-trips intact") {
    std::vector<uint8_t> data(256);
    for (uint16_t i = 0; i < 256; ++i)
      data[i] = static_cast<uint8_t>(i);
    const auto path = write_test_bundle({{"bin", data}});
    const bundlefile::Bundle bundle(path.string());
    const auto* root = bundle.get_bundle();
    const auto* file = root->files()->Get(0);
    REQUIRE(file->data()->size() == 256);
    for (size_t i = 0; i < 256; ++i)
      CHECK(file->data()->Get(i) == data[i]);
    std::filesystem::remove(path);
  }
}

TEST_SUITE("bundlefile::Bundle open errors") {
  TEST_CASE("missing file throws") {
    CHECK_THROWS_AS(
      bundlefile::Bundle("does_not_exist_bnd.bnd"), std::runtime_error
    );
  }
}
