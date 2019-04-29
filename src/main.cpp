// Copyright (c) 2018 by Tschokko.
// Author: tschokko

#include "messages.hpp"

#include <iostream>

#include "nlohmann/json.hpp"

int main(int argc, char* argv[]) {
  using json = nlohmann::json;
  auto welcome = nsyslcm::CreateWelcomeMessage(1234, json::object());

  if (welcome == nullptr) {
    std::cerr << "Protocol error" << std::endl;
    return 1;
  }

  std::cout << "Type: " << welcome->message_type() << std::endl;
  std::cout << "Session-ID: " << welcome->session_id() << std::endl;
  std::cout << "Detaiks: " << welcome->details() << std::endl;

  auto msg = nsyslcm::Unmarshal("[1,\"blaaa\", {}]");
  if (!msg) {
    std::cerr << "Next protocol error" << std::endl;
    return 1;
  }

  std::cout << "Type: " << (*msg)->message_type() << std::endl;

  return 0;
}
