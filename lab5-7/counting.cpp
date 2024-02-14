#include <unistd.h>

#include <chrono>
#include <sstream>
#include <unordered_map>

#include "timer.h"
#include "zmq_functions.h"

int main(int argc, char* argv[]) {
  if (argc != 2 && argc != 3) {
    throw std::runtime_error("Wrong args for counting node");
  }
  int cur_id = std::atoi(argv[1]);
  int child_id = -1;
  if (argc == 3) {
    child_id = std::atoi(argv[2]);
  }

  std::unordered_map<std::string, int> dictionary;

  zmq::context_t context;
  zmq::socket_t parent_socket(context, ZMQ_REP);  // создаем сокет-ответчик
  connectToNode(parent_socket, cur_id);

  zmq::socket_t child_socket(context, ZMQ_REQ);  // создаем сокет-запросчик
  child_socket.setsockopt(ZMQ_SNDTIMEO, 5000);
  if (child_id != -1) {
    bindToAddress(child_socket, child_id);
  }

  Timer timer;
  std::string message;
  while (true) {
    message = receiveMessage(parent_socket);
    std::istringstream request(message);
    int dest_id;
    request >> dest_id;  // получаем идентификатор получателя

    std::string cmd;
    request >> cmd;

    if (dest_id == cur_id) {
      if (cmd == "pid") {
        sendMessage(parent_socket, "OK: " + std::to_string(getpid()));
      }

      else if (cmd == "create") {
        int new_child_id;
        request >> new_child_id;
        if (child_id != -1) {
          unbindFromAddress(child_socket, child_id);
        }
        bindToAddress(child_socket, new_child_id);
        pid_t pid = fork();
        if (pid < 0) {
          perror("Can't create new process");
          return -1;
        }
        if (pid == 0) {
          execl("./counting", "./counting",
                std::to_string(new_child_id).c_str(),
                std::to_string(child_id).c_str(), NULL);
          perror("Can't execute new process");
          return -2;
        }
        sendMessage(child_socket, std::to_string(new_child_id) + "pid");
        child_id = new_child_id;
        sendMessage(parent_socket, receiveMessage(child_socket));
      } else if (cmd == "exec") {
        int sum = 0;
        std::string subcommand, answer;
        request >> subcommand;
        if (subcommand == "start") {
          timer.start();
          answer = "Ok: " + std::to_string(cur_id) + ", start timer";
        } else if (subcommand == "stop") {
          timer.stop();
          answer = "Ok: " + std::to_string(cur_id) + ", stop timer";
        } else if (subcommand == "time") {
          answer = "Ok: " + std::to_string(cur_id) + ", time is " +
                   std::to_string(timer.elapsedSeconds());
        } else {
          answer = "Ok: " + std::to_string(cur_id) + ", wrong command";
        }
        sendMessage(parent_socket, answer);
      }

      else if (cmd == "heartbeat") {
        std::string reply;
        if (child_id != -1) {
          sendMessage(child_socket, std::to_string(child_id) + " heartbeat");
          std::string msg = receiveMessage(child_socket);
          reply += " " + msg;
        }
        sendMessage(parent_socket, std::to_string(cur_id) + reply);
      } else if (cmd == "kill") {
        if (child_id != -1) {
          sendMessage(child_socket, std::to_string(child_id) + " kill");
          std::string msg = receiveMessage(child_socket);
          if (msg == "OK") {
            sendMessage(parent_socket, "OK");
          }
          unbindFromAddress(child_socket, child_id);
          disconnectFromNode(parent_socket, cur_id);
          break;
        }
        sendMessage(parent_socket, "OK");
        disconnectFromNode(parent_socket, cur_id);
        break;
      }
    } else if (child_id != -1) {
      sendMessage(child_socket, message);
      sendMessage(parent_socket, receiveMessage(child_socket));
      if (child_id == dest_id && cmd == "kill") {
        child_id = -1;
      }
    } else {
      sendMessage(parent_socket, "Error: node is unavailable");
    }
  }
}
