//
// Created by wolek on 29.06.24.
//

#ifndef OPENGLPLOTLIVE_PROJ_APPS_SELFBALACINGROBOTPLOT_UDPSERVER_HPP_
#define OPENGLPLOTLIVE_PROJ_APPS_SELFBALACINGROBOTPLOT_UDPSERVER_HPP_



namespace udp {

class UdpServer {
 public:
  ~UdpServer();
  explicit UdpServer(uint16_t udpPort);
  void reciveData(uint8_t* buff, size_t buffLeght);
 private:
  std::string port;
  int socketFd;
};

} // udp

#endif //OPENGLPLOTLIVE_PROJ_APPS_SELFBALACINGROBOTPLOT_UDPSERVER_HPP_
