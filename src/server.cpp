// Copyright (c) 2018 by Tschokko.
// Author: tschokko

#include "server.hpp"

#include <tuple>

#include "spdlog/spdlog.h"

namespace nsys::devicecontrol {

namespace log = spdlog;

Server::Server() {
  // set up access channels to only log interesting things
  endpoint_.clear_access_channels(websocketpp::log::alevel::all);
  endpoint_.set_access_channels(websocketpp::log::alevel::access_core);
  endpoint_.set_access_channels(websocketpp::log::alevel::app);

  // Initialize the Asio transport policy
  endpoint_.init_asio();

  // Bind the handlers we are using
  using websocketpp::lib::bind;
  using websocketpp::lib::placeholders::_1;
  using websocketpp::lib::placeholders::_2;
  endpoint_.set_open_handler(bind(&Server::OnOpen, this, _1));
  endpoint_.set_close_handler(bind(&Server::OnClose, this, _1));
  endpoint_.set_message_handler(bind(&Server::OnMessage, this, _1, _2));
}

void Server::Run(uint16_t port) {
  try {
    // listen on specified port
    endpoint_.listen(port);

    // Start the server accept loop
    endpoint_.start_accept();

    // Set the initial timer to start telemetry
    // set_timer();

    // Start the ASIO io_service run loop
    endpoint_.run();
  } catch (websocketpp::exception const& e) {
    log::error("Run server failed: {}", e.what());
  } catch (const std::exception& e) {
    log::error("Run server failed: {}", e.what());
  } catch (...) {
    log::error("Run server failed");
  }
};

void Server::OnOpen(connection_handle_t hdl) {
  // log::info("OnOpen called: {}", hdl.lock().get());
  {
    lock_guard<mutex> guard(actions_lock_);
    actions_.push(ServerAction(kServerActionSubscribe, hdl));
  }
  action_condition_.notify_one();
}

void Server::OnClose(connection_handle_t hdl) {
  // log::info("OnClose called: {}", hdl.lock().get());
  {
    lock_guard<mutex> guard(actions_lock_);
    actions_.push(ServerAction(kServerActionUnsubscribe, hdl));
  }
  action_condition_.notify_one();
}

void Server::OnMessage(connection_handle_t hdl, message_ptr_t msg) {
  using namespace websocketpp;

  log::info("Received message from {} with payload: {}", hdl.lock().get(),
            msg->get_payload());

  if (frame::opcode::is_control(msg->get_opcode())) {
    if (frame::opcode::close == msg->get_opcode()) {
      log::info("Session closed gracefully");
      return;
    }
  }

  // check for a special command to instruct the server to stop listening so
  // it can be cleanly exited.
  if (msg->get_payload() == "QUIT") {
    log::info("Close session gracefully: {}", hdl.lock().get());
    endpoint_.pause_reading(hdl);
    endpoint_.close(hdl, websocketpp::close::status::normal, "");
    return;
  }

  /*try {
    endpoint_.send(hdl, msg->get_payload(), msg->get_opcode());
  } catch (websocketpp::exception const& e) {
    log::error("Send message failed: {}", e.what());
  }*/
  {
    lock_guard<mutex> guard(actions_lock_);
    actions_.push(ServerAction(kServerActionMessage, hdl, msg));
  }
  action_condition_.notify_one();
}

void Server::Process() {
  static uint32_t id = 2000;

  while (1) {
    unique_lock<mutex> lock(actions_lock_);

    while (actions_.empty()) {
      action_condition_.wait(lock);
    }

    ServerAction a = actions_.front();
    actions_.pop();

    lock.unlock();

    switch (a.type) {
      case kServerActionSubscribe: {
        log::info("Insert connection: {}", a.hdl.lock().get());
        // lock_guard<mutex> guard(connections_lock_);
        // connections_.insert(a.hdl);

        lock_guard<mutex> guard(sessions_lock_);

        /*Session session;
        session.id = ++id;
        sessions_[a.hdl] = session;*/
        sessions_[a.hdl] = Session(++id);
      } break;
      case kServerActionUnsubscribe: {
        log::info("Erase connection: {}", a.hdl.lock().get());
        // lock_guard<mutex> guard(connections_lock_);
        // connections_.erase(a.hdl);

        lock_guard<mutex> guard(sessions_lock_);
        sessions_.erase(a.hdl);
      } break;
      case kServerActionMessage: {
        auto session = GetSessionForHandle(a.hdl);

        log::info("Process message for session {}: {}", session.id(),
                  a.hdl.lock().get());

        using std::ignore;
        lock_guard<mutex> guadrd(sessions_lock_);
        for (auto const& [hdl, ignore] : sessions_) {
          // log::info("Send message to session {}: {}", session.id, a.msg);
          endpoint_.send(hdl, a.msg);
        }

      } break;
      default:
        log::error("Unexpected server action type: {}", a.type);
    }

    log::info("Current connections: {}", sessions_.size());
  }
}

Session& Server::GetSessionForHandle(connection_handle_t hdl) {
  auto it = sessions_.find(hdl);

  if (it == sessions_.end()) {
    // this connection is not in the list. This really shouldn't happen
    // and probably means something else is wrong.
    throw std::invalid_argument("No data avaliable for session");
  }

  return it->second;
}


}  // namespace nsys::devicecontrol
