#include <format>
#include <iostream>
#include <memory>
#include <thread>

#include <args.hxx>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <net/tcp/client.hpp>
#include <net/tcp/socket.hpp>

#include "fbs/message.hpp"
#include "ui/chat.hpp"

class Client final : public net::TcpClient
{
public:
  bool running = true;

  std::string username;
  std::shared_ptr<ui::Chat> chat_ui;

  explicit Client(
    const std::shared_ptr<asio::io_context>& io_context,
    std::string user
  ) :
    net::TcpClient(io_context),
    username(std::move(user)),
    chat_ui(
      std::make_shared<ui::Chat>([&](const std::string& m) { on_input(m); })
    )
  {
  }

  std::shared_ptr<net::Socket> new_socket(asio::ip::tcp::socket socket) override
  {
    return std::make_shared<net::LengthPrefixedTcpSocket>(std::move(socket));
  }

  void connection_failed(const asio::error_code& code) override
  {
    chat_ui->add_message("Failed to connect to server: " + code.message());
    running = false;
  }

  void on_disconnect() override
  {
    chat_ui->add_message("Disconnected from server.");
    running = false;
  }

  void on_connect() override { chat_ui->add_message("Connected to server."); }

  void on_receive(const net::data_ptr_t& buffer) override
  {
    if (const auto* message = read_message(buffer))
      chat_ui->add_message(
        std::format(
          "[{}] {}", message->sender()->str(), message->content()->str()
        )
      );
  }

private:
  void on_input(const std::string& message)
  {
    if (message == "/clear")
    {
      chat_ui->clear_messages();
      return;
    }
    send(build_message(username, message));
  }
};

int main(
  int argc,
  char** argv
)
{
  args::ArgumentParser parser("Net demo chatroom client.");
  args::ValueFlag<std::string> user_arg(
    parser, "user", "Username.", {'u', "user"}, "anon"
  );
  args::ValueFlag<std::string> host_arg(
    parser, "host", "Server host.", {'h', "host"}, "127.0.0.1"
  );
  args::ValueFlag<std::uint16_t> port_arg(
    parser, "port", "Server port.", {'p', "port"}, 10000
  );
  try
  {
    parser.ParseCLI(argc, argv);
    const auto& user = args::get(user_arg);
    const auto& host = args::get(host_arg);
    const auto& port = args::get(port_arg);

    if (!glfwInit()) throw std::runtime_error("Failed to initialize GLFW.");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(
      900, 600, ("Chatroom - " + user).c_str(), nullptr, nullptr
    );
    if (!window)
    {
      glfwTerminate();
      throw std::runtime_error("Failed to create GLFW window.");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
      throw std::runtime_error("Failed to initialize GLAD.");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui::GetIO().IniFilename = nullptr;
    if (!ImGui_ImplGlfw_InitForOpenGL(window, true))
      throw std::runtime_error("Failed to initialize ImGui GLFW backend.");
    if (!ImGui_ImplOpenGL3_Init("#version 130"))
      throw std::runtime_error("Failed to initialize ImGui OpenGL3 backend.");

    const auto io_context = std::make_shared<asio::io_context>();
    Client client(io_context, user);

    std::thread io_thread([&]
    {
      try
      {
        io_context->run();
      }
      catch (const std::exception& e)
      {
        client.chat_ui->add_message(std::format("Error: {}", e.what()));
      }
    });

    client.chat_ui->add_message(
      std::format("Connecting to {}:{} as {}", host, port, user)
    );
    client.connect(host, port);

    while (!glfwWindowShouldClose(window))
    {
      glfwPollEvents();
      client.update();

      ImGui_ImplOpenGL3_NewFrame();
      ImGui_ImplGlfw_NewFrame();
      ImGui::NewFrame();

      const auto viewport = ImGui::GetMainViewport();
      ImGui::SetNextWindowPos(viewport->WorkPos);
      ImGui::SetNextWindowSize(viewport->WorkSize);
      ImGui::Begin(
        "Chatroom", nullptr,
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
          | ImGuiWindowFlags_NoCollapse
      );
      client.chat_ui->render(client.connected());
      ImGui::End();

      ImGui::Render();
      int display_w = 0;
      int display_h = 0;
      glfwGetFramebufferSize(window, &display_w, &display_h);
      glViewport(0, 0, display_w, display_h);
      glClearColor(0.12f, 0.12f, 0.14f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

      glfwSwapBuffers(window);
    }

    client.disconnect();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    if (!io_context->stopped()) io_context->stop();
    if (io_thread.joinable()) io_thread.join();
  }
  catch (const args::Completion& e)
  {
    std::cout << e.what();
  }
  catch (const args::Help&)
  {
    std::cout << parser;
  }
  catch (const args::Error& e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
