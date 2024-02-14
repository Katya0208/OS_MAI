#include <unistd.h>

#include <set>
#include <sstream>

#include "topology.h"
#include "zmq_functions.h"

int main() {
  topology network;
  std::vector<zmq::socket_t> branches;  // сокеты для подключения к узлам
  zmq::context_t context;

  std::string cmd;
  while (std::cin >> cmd) {
        if (cmd == "create") {
      int node_id, parent_id;
      std::cin >> node_id >> parent_id;
      if (parent_id != -1 && network.hasChildren(parent_id)) {
        std::cout << "Error: parent node has children" << std::endl;
      } else {
        if (network.find(node_id) != -1) {
          std::cout << "Error: already exists" << std::endl;
        } else if (parent_id == -1) {
          pid_t pid =
              fork();  // создаем дочерний процесс для вычислительных узлов
          if (pid < 0) {
            perror("Can't create new process");
            return -1;
          }
          if (pid == 0) {
            if (execl("./counting", "./counting",
                      std::to_string(node_id).c_str(), "-1", NULL) < 0) {
              perror("Can't execute new process");
              return -2;
            }
          }
          branches.emplace_back(
              context,
              ZMQ_REQ);  // создаем сокет для отправки запросов
                         // в сеть и помещаем в вектор сокетов
          branches.back().setsockopt(
              ZMQ_SNDTIMEO,
              5000);  // устанавливаем таймаут ожидания отправки сообщения
                      // (5сек) последнему сокету
          bindToAddress(branches.back(),
                        node_id);  // привязываем сокет к адресу
          sendMessage(
              branches.back(),
              std::to_string(node_id) +
                  "pid");  // отправляем сообщение с идентификатором узла

          std::string reply =
              receiveMessage(branches.back());  // получаем ответ
          std::cout << reply << std::endl;
          network.insert(node_id, parent_id);  // добавляем узел в сеть
        } else if (network.find(parent_id) == -1) {
          std::cout << "Error: parent not found" << std::endl;
        } else {
          int branch = network.find(parent_id);
          sendMessage(branches[branch], std::to_string(parent_id) + " create " +
                                            std::to_string(node_id));

          std::string reply = receiveMessage(branches[branch]);
          std::cout << reply << std::endl;
          network.insert(node_id, parent_id);
        }
      }
    } else if (cmd == "exec") {
      int dest_id;
      std::string numbers;
      std::cin >> dest_id;
      std::getline(std::cin >> std::ws,
                   numbers);  // пропускаем пробелы и считываем числа
      int branch = network.find(dest_id);
      if (branch == -1) {
        std::cout << "ERROR: incorrect node id" << std::endl;
      } else {
        sendMessage(branches[branch],
                    std::to_string(dest_id) + " exec " + numbers);
        std::string reply = receiveMessage(branches[branch]);
        std::cout << reply << std::endl;
      }
    } else if (cmd == "kill") {
      int id;
      std::cin >> id;
      int branch = network.find(id);
      if (branch == -1) {
        std::cout << "ERROR: incorrect node id" << std::endl;
      } else {
        bool is_first =
            (network.get_first_id(branch) ==
             id);  // проверяем, является ли идентификатор первым узлом в списке
        sendMessage(branches[branch], std::to_string(id) + " kill");

        std::string reply = receiveMessage(branches[branch]);
        std::cout << reply << std::endl;
        network.erase(id);
        if (is_first) {
          unbindFromAddress(branches[branch], id);
          branches.erase(branches.begin() + branch);
        }
      }
    } else if (cmd == "heartbeat") {
      std::set<int> available_nodes;
      for (size_t i = 0; i < branches.size(); ++i) {
        int first_node_id = network.get_first_id(i);
        sendMessage(branches[i], std::to_string(first_node_id) + " heartbeat");

        std::string received_message = receiveMessage(branches[i]);
        std::istringstream reply(received_message);
        int node;
        while (reply >> node) {
          available_nodes.insert(
              node);  // сохраняем доступные (уникальные) узлы
        }
      }
      std::cout << "OK: ";
      if (available_nodes.empty()) {
        std::cout << "No available nodes" << std::endl;
      } else {
        for (auto v : available_nodes) {
          std::cout << v << " ";
        }
        std::cout << std::endl;
      }
    } else if (cmd == "exit") {
      for (size_t i = 0; i < branches.size(); ++i) {
        int first_node_id = network.get_first_id(i);
        sendMessage(branches[i], std::to_string(first_node_id) + " kill");
        std::string reply = receiveMessage(branches[i]);
        if (reply != "OK") {
          std::cout << reply << std::endl;
        } else {
          unbindFromAddress(branches[i], first_node_id);
        }
      }
      exit(0);
    } else {
      std::cout << "Incorrect cmd" << std::endl;
    }
  }
}
