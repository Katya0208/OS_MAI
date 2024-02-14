#include <iostream>
#include <zmq.hpp>

#define MAIN_PORT 6060

void sendMessage(zmq::socket_t& socket, const std::string& msg) {
  zmq::message_t message(msg.size());  // создается объект сообщение, которое
                                       // будет отправлено через сокет
  memcpy(message.data(), msg.c_str(), msg.size());
  socket.send(message);
}

std::string receiveMessage(zmq::socket_t& socket) {
  zmq::message_t message;
  bool charsRead = false;  // статус успешности приема сообщения
  try {
    charsRead =
        socket.recv(&message);  // извлекаем сообщение из сокета и сохраняем
  } catch (const zmq::error_t& e) {
    std::cerr << "ZeroMQ error: " << e.what() << std::endl;
  }
  if (!charsRead) {
    return "Error: failed to receive message from node";
  }
  std::string receivedMsg(
      static_cast<char*>(message.data()),
      message
          .size());  // копируем сообщение в строку с помощьюпреобразования
                     // указателя message.data() в указатель на массив символов
  return receivedMsg;
}

void connectToNode(zmq::socket_t& socket, int id) {
  std::string address = "tcp://127.0.0.1:" + std::to_string(MAIN_PORT + id);
  try {
    socket.connect(address);
  } catch (const zmq::error_t& e) {
    std::cerr << "ZeroMQ error: " << e.what() << std::endl;
  }
}

void disconnectFromNode(zmq::socket_t& socket, int id) {
  std::string address = "tcp://127.0.0.1:" + std::to_string(MAIN_PORT + id);
  try {
    socket.disconnect(address);
  } catch (const zmq::error_t& e) {
    std::cerr << "ZeroMQ error: " << e.what() << std::endl;
  }
}

void bindToAddress(zmq::socket_t& socket,
                   int id) {  // привязка сокета к указанному адресу, чтобы
                              // другие узлы могли подключиться к этому сокету
  std::string address = "tcp://127.0.0.1:" + std::to_string(MAIN_PORT + id);
  try {
    socket.bind(address);
  } catch (const zmq::error_t& e) {
    std::cerr << "ZeroMQ error: " << e.what() << std::endl;
  }
}

void unbindFromAddress(zmq::socket_t& socket, int id) {
  std::string address = "tcp://127.0.0.1:" + std::to_string(MAIN_PORT + id);
  try {
    socket.unbind(address);
  } catch (const zmq::error_t& e) {
    std::cerr << "ZeroMQ error: " << e.what() << std::endl;
  }
}
