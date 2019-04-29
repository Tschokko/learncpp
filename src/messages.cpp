// Copyright (c) 2018 by Tschokko.
// Author: tschokko
//
// Most used methods
// Factory::Unmarshal(string) returns a message object
// Factory::Marshal(Message) returns JSON stringified object
// Factory::CreateWelcomeMessage(...) returns a new welcome message object
// Message->Marshal() returns JSON stringified object

#include "messages.hpp"

#include <iostream>
#include <optional>

#include "absl/memory/memory.h"

namespace lcm {
namespace protocol {

using json = nlohmann::json;

std::unique_ptr<WelcomeMessage> MessageFactory::CreateWelcomeMessage(
    uint32_t session_id, json::object_t details) {
  return absl::WrapUnique(new WelcomeMessage(session_id, details));
}

std::unique_ptr<WelcomeMessage> CreateWelcomeMessage(uint32_t session_id,
                                                     json::object_t details) {
  return MessageFactory::CreateWelcomeMessage(session_id, details);
}

std::optional<std::unique_ptr<BaseMessage>> MessageFactory::Unmarshal(
    const std::string_view& v) {
  auto msg = json::parse(v);

  if (!msg.is_array() && msg.size() == 0) {
    return nullptr;
  }

  auto msg_type = msg.at(0);
  if (!msg_type.is_number()) {
    return nullptr;
  }

  switch (static_cast<int>(msg_type)) {
    case kHelloMessageType:
      return absl::WrapUnique(new HelloMessage(msg.at(1), msg.at(2)));
    case kWelcomeMessageType:
    default:
      return absl::WrapUnique(new BaseMessage);
  }
}

}  // namespace protocol
}  // namespace lcm

int main(int argc, char* argv[]) {
  using json = nlohmann::json;
  auto welcome = lcm::protocol::CreateWelcomeMessage(1234, json::object());

  if (welcome == nullptr) {
    std::cerr << "Protocol error" << std::endl;
    return 1;
  }

  std::cout << "Type: " << welcome->message_type() << std::endl;
  std::cout << "Session-ID: " << welcome->session_id() << std::endl;
  std::cout << "Detaiks: " << welcome->details() << std::endl;

  auto msg = lcm::protocol::MessageFactory::Unmarshal("[1,\"blaaa\", {}]");
  if (!msg) {
    std::cerr << "Next protocol error" << std::endl;
    return 1;
  }

  std::cout << "Type: " << (*msg)->message_type() << std::endl;

  return 0;
}
