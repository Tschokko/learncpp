// Copyright (c) 2018 by Tschokko.
// Author: tschokko

#ifndef SRC_SERVER_HPP_
#define SRC_SERVER_HPP_

#define ASIO_STANDALONE

#include <iostream>
#include <optional>
#include <set>

#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"

namespace nsys::devicecontrol {

using websocketpp::lib::condition_variable;
using websocketpp::lib::lock_guard;
using websocketpp::lib::mutex;
using websocketpp::lib::thread;
using websocketpp::lib::unique_lock;

typedef websocketpp::connection_hdl connection_handle_t;
typedef websocketpp::server<websocketpp::config::asio> server_t;
typedef server_t::message_ptr message_ptr_t;
typedef std::set<connection_handle_t, std::owner_less<connection_handle_t>>
    connection_list_t;

enum ServerActionTypes {
  kServerActionSubscribe,
  kServerActionUnsubscribe,
  kServerActionMessage
};

struct ServerAction {
  ServerAction(ServerActionTypes t, connection_handle_t h) : type(t), hdl(h) {}
  ServerAction(ServerActionTypes t, connection_handle_t h, message_ptr_t m)
      : type(t), hdl(h), msg(m) {}

  ServerActionTypes type;
  connection_handle_t hdl;
  message_ptr_t msg;
};

class Session {
 public:
  Session() : id_(0) {}
  explicit Session(uint32_t id) : id_(id) {}
  uint32_t id() { return id_; }

 private:
  uint32_t id_;
};

typedef std::map<connection_handle_t, Session,
                 std::owner_less<connection_handle_t>>
    session_map_t;

class Server {
 public:
  Server();

  void Run(uint16_t port);
  void Process();

 private:
  server_t endpoint_;
  // mutex connections_lock_;
  // connection_list_t connections_;
  mutex actions_lock_;
  std::queue<ServerAction> actions_;
  condition_variable action_condition_;
  mutex sessions_lock_;
  session_map_t sessions_;

  void OnOpen(connection_handle_t hdl);
  void OnClose(connection_handle_t hdl);
  void OnMessage(connection_handle_t hdl, message_ptr_t msg);
  auto GetSessionForHandle(connection_handle_t hdl) {
    auto it = sessions_.find(hdl);
    return (it == sessions_.end())
               ? std::nullopt
               : std::optional<std::reference_wrapper<Session>>{it->second};
  };
};

}  // namespace nsys::devicecontrol

#endif  // SRC_SERVER_HPP_