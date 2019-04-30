// Copyright (c) 2018 by Tschokko.
// Author: tschokko

#ifndef SRC_MESSAGES_HPP_
#define SRC_MESSAGES_HPP_

#include <string>

#include "nlohmann/json.hpp"

namespace nsys::devicecontrolv1 {

using json = nlohmann::json;

enum MessageTypes {
  kUnknownMessageType = 0,
  kHelloMessageType = 1,
  kWelcomeMessageType = 2
};

class MessageFactory;

class BaseMessage {
 public:
  // Disable copy (and move) semantics.
  BaseMessage(const BaseMessage&) = delete;
  BaseMessage& operator=(const BaseMessage&) = delete;

  MessageTypes message_type() { return message_type_; }

 protected:
  friend MessageFactory;
  BaseMessage() : message_type_(kUnknownMessageType) {}

  void set_message_type(MessageTypes message_type) {
    message_type_ = message_type;
  }

  MessageTypes message_type_;
};

class HelloMessage : public BaseMessage {
 public:
  static std::optional<std::unique_ptr<HelloMessage>> Parse(const json& j);

  std::string realm() { return realm_; }
  json details() { return details_; }

 protected:
  friend MessageFactory;
  HelloMessage(std::string realm, const json& details)
      : realm_(realm), details_(details) {
    set_message_type(kHelloMessageType);
  }
  explicit HelloMessage(std::string realm)
      : HelloMessage(realm, json::object()) {}

 private:
  std::string realm_;
  json details_;
};

class WelcomeMessage : public BaseMessage {
 public:
  static std::optional<std::unique_ptr<WelcomeMessage>> Parse(const json& j);

  uint32_t session_id() { return session_id_; }
  json details() { return details_; }

 protected:
  friend MessageFactory;
  WelcomeMessage(uint32_t session_id, const json& details)
      : session_id_(session_id), details_(details) {
    set_message_type(kWelcomeMessageType);
  }
  explicit WelcomeMessage(uint32_t session_id)
      : WelcomeMessage(session_id, json::object()) {}

 private:
  uint32_t session_id_;
  json details_;
};

class MessageFactory {
 public:
  static std::unique_ptr<HelloMessage> CreateHelloMessage(std::string realm,
                                                          const json& details);
  static std::unique_ptr<WelcomeMessage> CreateWelcomeMessage(
      uint32_t session_id, const json& details);

  static std::optional<std::unique_ptr<BaseMessage>> UnmarshalMessage(
      const std::string_view& v);

  // Disable copy (and move) semantics.
  MessageFactory(const MessageFactory&) = delete;
  MessageFactory& operator=(const MessageFactory&) = delete;

 private:
  MessageFactory();
};

std::unique_ptr<WelcomeMessage> CreateWelcomeMessage(uint32_t session_id,
                                                     const json& details);
std::unique_ptr<HelloMessage> CreateHelloMessage(std::string realm,
                                                 const json& details);

std::optional<std::unique_ptr<BaseMessage>> UnmarshalMessage(
    const std::string_view& v);

}  // namespace nsys::devicecontrolv1

#endif  // SRC_MESSAGES_HPP_
