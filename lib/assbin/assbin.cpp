
#include <iostream>

#include <spdlog/spdlog.h>
#include <args.hxx>

#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

int convert(const std::string& input, const std::string& output, unsigned int flag)
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
  args::HelpFlag help(parser, "help", "Display the help menu.", { 'h', "help" });

  args::Positional<std::string> source(parser, "source", "The source model file.", args::Options::Required);
  args::Positional<std::string> target(parser, "target", "The output assbin file.", args::Options::Required);
  args::Flag embed_textures(parser, "embed", "Embed texture data to output binary file.", { 'e', "embed" });

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
    aiProcess_FixInfacingNormals |
    aiProcess_FindDegenerates |
    aiProcess_FindInvalidData |
    aiProcess_OptimizeMeshes |
    aiProcess_FlipWindingOrder;

  if (embed_textures)
  {
    SPDLOG_INFO("Embed textures: enabled");
    flag |= aiProcess_EmbedTextures;
  }

  return convert(args::get(source), args::get(target), flag);
}
