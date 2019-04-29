// Copyright (c) 2018 by Tschokko.
// Author: tschokko

#ifndef SRC_MESSAGES_HPP_
#define SRC_MESSAGES_HPP_

#include <string>

#include "nlohmann/json.hpp"

namespace nsyslcm {

using json = nlohmann::json;

enum MessageTypes {
  kUnknownMessageType = 0,
  kHelloMessageType = 1,
  kWelcomeMessageType = 2
};

class MessageFactory;

class BaseMessage {
 public:
  // BaseMessage and inheritated classes are not copyable.
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
  std::string realm() { return realm_; }
  json details() { return details_; }

 protected:
  friend MessageFactory;
  HelloMessage(std::string realm, json details)
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
  uint32_t session_id() { return session_id_; }
  json details() { return details_; }

 protected:
  friend MessageFactory;
  WelcomeMessage(uint32_t session_id, json details)
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
  static std::unique_ptr<WelcomeMessage> CreateWelcomeMessage(
      uint32_t session_id, json::object_t details);
  static std::optional<std::unique_ptr<BaseMessage>> Unmarshal(
      const std::string_view& v);

  // MessageFactory is not copyable.
  MessageFactory(const MessageFactory&) = delete;
  MessageFactory& operator=(const MessageFactory&) = delete;

 private:
  MessageFactory();
};

std::unique_ptr<WelcomeMessage> CreateWelcomeMessage(uint32_t session_id,
                                                     json::object_t details);
std::optional<std::unique_ptr<BaseMessage>> Unmarshal(
    const std::string_view& v);

}  // namespace nsyslcm

#endif  // SRC_MESSAGES_HPP_
