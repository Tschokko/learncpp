// Copyright (c) 2018 by Tschokko.
// Author: tschokko
//
// Most used methods
// Factory::Unmarshal(string) returns a message object
// Factory::Marshal(Message) returns JSON stringified object
// Factory::CreateWelcomeMessage(...) returns a new welcome message object
// Message->Marshal() returns JSON stringified object

#include "messages.hpp"

#include <optional>

#include "absl/memory/memory.h"

namespace nsys::devicecontrolv1 {

using json = nlohmann::json;

static inline bool IsValidJsonMessage(const json& j,
                                      MessageTypes expected_msg_type) {
  // We expect an array with at least one element, the message type
  if (!j.is_array() && j.size() == 0) {
    return false;
  }

  // Message type should be an integer
  auto msg_type = j.at(0);
  if (!msg_type.is_number_integer()) {
    return false;
  }

  // Message type should be a hello message
  if (static_cast<int>(msg_type) != expected_msg_type) {
    return false;
  }

  return true;
}

std::optional<std::unique_ptr<HelloMessage>> HelloMessage::Parse(
    const json& j) {
  if (!IsValidJsonMessage(j, kHelloMessageType)) {
    return nullptr;
  }

  // Realm should be a string
  if (!j.at(1).is_string()) {
    return nullptr;
  }

  // Everything is fine, let's create a hello message
  return absl::WrapUnique(new HelloMessage(j.at(1), j.at(2)));
}

std::optional<std::unique_ptr<WelcomeMessage>> WelcomeMessage::Parse(
    const json& j) {
  if (!IsValidJsonMessage(j, kWelcomeMessageType)) {
    return nullptr;
  }

  // Session ID should be an unsigned integer
  if (!j.at(1).is_number_unsigned()) {
    return nullptr;
  }

  // Everything is fine, let's create a welcome message
  return absl::WrapUnique(new WelcomeMessage(j.at(1), j.at(2)));
}

std::unique_ptr<HelloMessage> MessageFactory::CreateHelloMessage(
    std::string realm, const json& details) {
  return absl::WrapUnique(new HelloMessage(realm, details));
}

std::unique_ptr<HelloMessage> CreateHelloMessage(std::string realm,
                                                 const json& details) {
  return MessageFactory::CreateHelloMessage(realm, details);
}

std::unique_ptr<WelcomeMessage> MessageFactory::CreateWelcomeMessage(
    uint32_t session_id, const json& details) {
  return absl::WrapUnique(new WelcomeMessage(session_id, details));
}

std::unique_ptr<WelcomeMessage> CreateWelcomeMessage(uint32_t session_id,
                                                     const json& details) {
  return MessageFactory::CreateWelcomeMessage(session_id, details);
}

std::optional<std::unique_ptr<BaseMessage>> MessageFactory::UnmarshalMessage(
    const std::string_view& v) {
  auto j = json::parse(v);

  // Let's look ahead into the message to decide which message to umarshal.

  // We expect an array with at least one element, the message type.
  if (!j.is_array() && j.size() == 0) {
    return nullptr;
  }

  // Message type should be an integer value
  auto msg_type = j.at(0);
  if (!msg_type.is_number_integer()) {
    return nullptr;
  }

  // Unmarshal the message by message type
  switch (static_cast<int>(msg_type)) {
    case kHelloMessageType: {
      return HelloMessage::Parse(j);
    }
    case kWelcomeMessageType: {
      return WelcomeMessage::Parse(j);
    }
    default:
      return nullptr;
  }
}

std::optional<std::unique_ptr<BaseMessage>> UnmarshalMessage(
    const std::string_view& v) {
  return MessageFactory::UnmarshalMessage(v);
}

}  // namespace nsys::devicecontrolv1
