
#include <iostream>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/bundled/color.h>
#include <args.hxx>

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

static std::string option_string(unsigned int flag, aiPostProcessSteps option, const std::string& name)
{
  bool enabled = flag & option;

  return name + ": " + fmt::format(
    fmt::fg(enabled ? fmt::terminal_color::green : fmt::terminal_color::red),
    enabled ? "enabled" : "disabled") + '\n';
}

static int convert(const std::string& input, const std::string& output, unsigned int flag)
{
  std::stringstream options_ss;
  options_ss
    << option_string(flag, aiProcess_CalcTangentSpace, "CalcTangentSpace")
    << option_string(flag, aiProcess_JoinIdenticalVertices, "JoinIdenticalVertices")
    << option_string(flag, aiProcess_MakeLeftHanded, "MakeLeftHanded")
    << option_string(flag, aiProcess_Triangulate, "Triangulate")
    << option_string(flag, aiProcess_RemoveComponent, "RemoveComponent")
    << option_string(flag, aiProcess_GenNormals, "GenNormals")
    << option_string(flag, aiProcess_GenSmoothNormals, "GenSmoothNormals")
    << option_string(flag, aiProcess_SplitLargeMeshes, "SplitLargeMeshes")
    << option_string(flag, aiProcess_PreTransformVertices, "PreTransformVertices")
    << option_string(flag, aiProcess_LimitBoneWeights, "LimitBoneWeights")
    << option_string(flag, aiProcess_ValidateDataStructure, "ValidateDataStructure")
    << option_string(flag, aiProcess_ImproveCacheLocality, "ImproveCacheLocality")
    << option_string(flag, aiProcess_RemoveRedundantMaterials, "RemoveRedundantMaterials")
    << option_string(flag, aiProcess_FixInfacingNormals, "FixInfacingNormals")
    << option_string(flag, aiProcess_PopulateArmatureData, "PopulateArmatureData")
    << option_string(flag, aiProcess_SortByPType, "SortByPType")
    << option_string(flag, aiProcess_FindDegenerates, "FindDegenerates")
    << option_string(flag, aiProcess_FindInvalidData, "FindInvalidData")
    << option_string(flag, aiProcess_GenUVCoords, "GenUVCoords")
    << option_string(flag, aiProcess_TransformUVCoords, "TransformUVCoords")
    << option_string(flag, aiProcess_FindInstances, "FindInstances")
    << option_string(flag, aiProcess_OptimizeMeshes, "OptimizeMeshes")
    << option_string(flag, aiProcess_OptimizeGraph, "OptimizeGraph")
    << option_string(flag, aiProcess_FlipUVs, "FlipUVs")
    << option_string(flag, aiProcess_FlipWindingOrder, "FlipWindingOrder")
    << option_string(flag, aiProcess_SplitByBoneCount, "SplitByBoneCount")
    << option_string(flag, aiProcess_Debone, "Debone")
    << option_string(flag, aiProcess_GlobalScale, "GlobalScale")
    << option_string(flag, aiProcess_EmbedTextures, "EmbedTextures")
    << option_string(flag, aiProcess_ForceGenNormals, "ForceGenNormals")
    << option_string(flag, aiProcess_DropNormals, "DropNormals")
    << option_string(flag, aiProcess_GenBoundingBoxes, "GenBoundingBoxes");
  SPDLOG_INFO("Options:\n{}", options_ss.str());

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
  args::HelpFlag help(parser, "help", "Display the help menu.", { 'h', "help" });

  args::Positional<std::string> source(parser, "source", "The source model file.", args::Options::Required);
  args::Positional<std::string> target(parser, "target", "The output assbin file.", args::Options::Required);

  args::Flag embed_textures(parser, "embed textures",
    "Embed texture data to output binary file.",
    { "embed-texture" });

  args::Flag flip_uv(parser, "flip uv",
    "Flips all UV coordinates along the y-axis and adjusts material settings and bitangents accordingly.",
    { "flip-uv" });

  try { parser.ParseCLI(argc, argv); }
  catch (const args::Completion& e) { std::cout << e.what(); }
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

  unsigned int flag =
    aiProcess_CalcTangentSpace |
    aiProcess_JoinIdenticalVertices |
    aiProcess_Triangulate |
    aiProcess_GenSmoothNormals |
    aiProcess_PreTransformVertices |
    aiProcess_ValidateDataStructure |
    aiProcess_RemoveRedundantMaterials |
    aiProcess_GenUVCoords |
    aiProcess_TransformUVCoords |
    aiProcess_FixInfacingNormals |
    aiProcess_FindDegenerates |
    aiProcess_FindInvalidData |
    aiProcess_OptimizeMeshes |
    aiProcess_FlipWindingOrder;

  if (embed_textures)
    flag |= aiProcess_EmbedTextures;

  if (flip_uv)
    flag |= aiProcess_FlipUVs;

  return convert(args::get(source), args::get(target), flag);
}
