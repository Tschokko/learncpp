// Copyright (c) 2018 by Tschokko.
// Author: tschokko

/*#include "messages.hpp"

#include <iostream>

#include "nlohmann/json.hpp"*/

#include "server.hpp"

#include "spdlog/spdlog.h"
#include "websocketpp/common/thread.hpp"

using websocketpp::lib::bind;
using websocketpp::lib::thread;

volatile std::sig_atomic_t gSignalStatus;

void SignalHandler(int signal) {
  std::cout << "SIGINT received" << std::endl;
  gSignalStatus = signal;
  // server.Shutdown();
}

int main(/*int argc, char* argv[]*/) {
  // std::signal(SIGINT, SignalHandler);

  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
  spdlog::info("Starting device control server");

  using namespace nsys;
  devicecontrol::Server server;

  // Start a thread to run the processing loop
  thread t(bind(&devicecontrol::Server::Process, &server));

  // Run the asio loop with the main thread
  server.Run(4001);

  t.join();

  /*using json = nlohmann::json;
  auto welcome =
      nsys::devicecontrolv1::CreateWelcomeMessage(1234, json::object());

  std::cout << "Type: " << welcome->message_type() << std::endl;
  std::cout << "Session-ID: " << welcome->session_id() << std::endl;
  std::cout << "Detaiks: " << welcome->details() << std::endl;


  auto result = nsys::devicecontrolv1::HelloMessage::Parse(json::array());
  if (result.has_error()) {
    std::cerr << "Next protocol error: " << result.error() << std::endl;
    return 1;
  }

  auto hello = result.Unwrap();
  std::cout << "Type: " << hello->message_type() << std::endl;*/


  /*int rc = 0;
  auto msg = nsys::devicecontrolv1::UnmarshalMessage("[1,\"blaaa\", {}]",
  &rc); if (!msg) { std::cerr << "Next protocol error" << std::endl; return
  1;
  }

  std::cout << "Type: " << (*msg)->message_type() << std::endl;*/

  return 0;
}
