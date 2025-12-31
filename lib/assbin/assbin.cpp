
#include <args.hxx>
#include <spdlog/fmt/bundled/color.h>
#include <spdlog/spdlog.h>

#include <assimp/Exporter.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#define FOR_EACH_OPTIONS(macro, arg)                                                                                   \
  macro(CalcTangentSpace, arg);                                                                                        \
  macro(JoinIdenticalVertices, arg);                                                                                   \
  macro(MakeLeftHanded, arg);                                                                                          \
  macro(Triangulate, arg);                                                                                             \
  macro(RemoveComponent, arg);                                                                                         \
  macro(GenNormals, arg);                                                                                              \
  macro(GenSmoothNormals, arg);                                                                                        \
  macro(SplitLargeMeshes, arg);                                                                                        \
  macro(PreTransformVertices, arg);                                                                                    \
  macro(LimitBoneWeights, arg);                                                                                        \
  macro(ValidateDataStructure, arg);                                                                                   \
  macro(ImproveCacheLocality, arg);                                                                                    \
  macro(RemoveRedundantMaterials, arg);                                                                                \
  macro(FixInfacingNormals, arg);                                                                                      \
  macro(PopulateArmatureData, arg);                                                                                    \
  macro(SortByPType, arg);                                                                                             \
  macro(FindDegenerates, arg);                                                                                         \
  macro(FindInvalidData, arg);                                                                                         \
  macro(GenUVCoords, arg);                                                                                             \
  macro(TransformUVCoords, arg);                                                                                       \
  macro(FindInstances, arg);                                                                                           \
  macro(OptimizeMeshes, arg);                                                                                          \
  macro(OptimizeGraph, arg);                                                                                           \
  macro(FlipUVs, arg);                                                                                                 \
  macro(FlipWindingOrder, arg);                                                                                        \
  macro(SplitByBoneCount, arg);                                                                                        \
  macro(Debone, arg);                                                                                                  \
  macro(GlobalScale, arg);                                                                                             \
  macro(EmbedTextures, arg);                                                                                           \
  macro(ForceGenNormals, arg);                                                                                         \
  macro(DropNormals, arg);                                                                                             \
  macro(GenBoundingBoxes, arg);

#define DEFINE_ASSIMP_OPTION(option, parser) args::Flag option(parser, #option, #option, {#option});

#define USE_ASSIMP_OPTION(option, flag)                                                                                \
  SPDLOG_INFO(                                                                                                         \
    "{}: {}", #option,                                                                                                 \
    fmt::format(                                                                                                       \
      fmt::fg(((flag) & aiProcess_##option) ? fmt::terminal_color::green : fmt::terminal_color::red),                  \
      ((flag) & aiProcess_##option) ? "enabled" : "disabled"));                                                        \
  if (option) (flag) |= aiProcess_##option;

static int convert(const std::string& input, const std::string& output, unsigned int flag)
{
  Assimp::Importer importer;
  const auto* scene = importer.ReadFile(input, flag);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    SPDLOG_ERROR("Failed to load model: {}", importer.GetErrorString());
    return 1;
  }

  Assimp::Exporter exporter;
  return exporter.Export(scene, "assbin", output);
}

int main(int argc, char** argv)
{
  args::ArgumentParser parser("Convert a 3d model to assimp binary format.");
  args::HelpFlag help(parser, "help", "Display the help menu.", {'h', "help"});

  args::Positional<std::string> source(parser, "source", "The source model file.", args::Options::Required);
  args::Positional<std::string> target(parser, "target", "The output assbin file.", args::Options::Required);

  DEFINE_ASSIMP_OPTION(PresetTargetRealtime, parser);
  FOR_EACH_OPTIONS(DEFINE_ASSIMP_OPTION, parser);

  try
  {
    parser.ParseCLI(argc, argv);
  }
  catch (const args::Completion& e)
  {
    std::cout << e.what();
  }
  catch (const args::Help&)
  {
    std::cout << parser;
    return 0;
  }
  catch (const args::Error& e)
  {
    SPDLOG_ERROR(e.what());
    return 1;
  }

  unsigned int flag = 0;
  if (PresetTargetRealtime) flag |= aiProcessPreset_TargetRealtime_MaxQuality;

  FOR_EACH_OPTIONS(USE_ASSIMP_OPTION, flag);

  return convert(args::get(source), args::get(target), flag);
}
