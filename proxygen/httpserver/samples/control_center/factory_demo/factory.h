#include "../../../../../CSerialPort/include/CSerialPort/SerialPort.h"
#include "../../../../../CSerialPort/include/CSerialPort/SerialPortInfo.h"
#include <atomic>
#include <iostream>
using namespace itas109;

#include <vector>
using namespace std;

#ifdef I_OS_WIN
#define imsleep(x) Sleep(x)
#elif defined I_OS_UNIX // unix
#define imsleep(x) usleep(1000 * x)
#else
#define imsleep(x)
#endif

enum AGVCarStatus { arm2, arm3, moving };

enum RobotArmStatus { wait, ready, startPick, finish };

enum ConveyorStatus { running, stopping };

class AGVCar {
 private:
  AGVCarStatus status;

 public:
  AGVCar() {
  }

  AGVCar(AGVCarStatus status_) : status(status_) {
  }

  ~AGVCar() {
  }

  void init(AGVCarStatus status_) {
    this->status = status_;
  }

  AGVCarStatus getStatus() {
    return status;
  }

  void setStatus(AGVCarStatus status_) {
    this->status = status_;
  }
};

class RobotArm {
 private:
  RobotArmStatus status;
  std::atomic_flag lock = ATOMIC_FLAG_INIT;

 public:
  RobotArm() {
  }

  RobotArm(RobotArmStatus status_) : status(status_) {
  }

  ~RobotArm() {
  }

  void init(RobotArmStatus status_) {
    this->status = status_;
  }

  RobotArmStatus getStatus() {
    return status;
  }

  void setStatus(RobotArmStatus status_) {
    this->status = status_;
  }

  bool getSetIsStatus(RobotArmStatus isStatus_, RobotArmStatus status_) {
    bool res = false;
    while (lock.test_and_set())
      ;
    if (this->status == isStatus_) {
      this->status = status_;
      res = true;
    }
    lock.clear();
    return res;
  }

  bool getSetNotStatus(RobotArmStatus notStatus_, RobotArmStatus status_) {
    bool res = false;
    while (lock.test_and_set())
      ;
    if (this->status != notStatus_) {
      this->status = status_;
      res = true;
    }
    lock.clear();
    return res;
  }
};

class Conveyor {
 private:
  ConveyorStatus status;
  CSerialPort m_serialport;
  //   std::string portName = "/dev/ttyS5";
  std::string portName = "/dev/pts/0";
  char serial_sendmsg[8] = {0};

 public:
  Conveyor() {
  }

  Conveyor(ConveyorStatus status_) {
    this->status = status_;
  }

  ~Conveyor() {
  }

  void init(ConveyorStatus status_) {
    this->status = status_;
    this->m_serialport.init(this->portName);
    this->m_serialport.open();
    this->switchConveyor(ConveyorStatus::running);
  }

  ConveyorStatus getStatus() {
    return status;
  }

  void setStatus(ConveyorStatus status_) {
    switchConveyor(status_);
    this->status = status_;
  }

  bool switchConveyor(ConveyorStatus status) {
    if (status == ConveyorStatus::running) {
      std::cout << "switch on conveyor" << std::endl;

      this->serial_sendmsg[0] = 0xFE;
      this->serial_sendmsg[1] = 0x05;
      this->serial_sendmsg[2] = 0x00;
      this->serial_sendmsg[3] = 0x01;
      this->serial_sendmsg[4] = 0xFF;
      this->serial_sendmsg[5] = 0x00;
      this->serial_sendmsg[6] = 0xC9;
      this->serial_sendmsg[7] = 0xF5;

      this->m_serialport.writeData(serial_sendmsg, sizeof(serial_sendmsg));

      return true;
    } else if (status == ConveyorStatus::stopping) {
      std::cout << "switch off conveyor" << std::endl;

      this->serial_sendmsg[0] = 0xFE;
      this->serial_sendmsg[1] = 0x05;
      this->serial_sendmsg[2] = 0x00;
      this->serial_sendmsg[3] = 0x01;
      this->serial_sendmsg[4] = 0x00;
      this->serial_sendmsg[5] = 0x00;
      this->serial_sendmsg[6] = 0x88;
      this->serial_sendmsg[7] = 0x05;

      this->m_serialport.writeData(serial_sendmsg, sizeof(serial_sendmsg));

      return true;
    }
    return false;
  }

  bool serialIsOpened() {
    return this->m_serialport.isOpened();
  }

  void closeSerial() {
    this->m_serialport.close();
  }
};

class Factory {
 private:
  int AGVCarNumber;
  int robotArmNumber;
  int conveyorNumber;
  RobotArm *robotArm;
  AGVCar *agvCar;
  Conveyor *conveyor;

 public:
  Factory(int AGVCarNumber_ = 2,
          int robotArmNumber_ = 3,
          int conveyorNumber_ = 1)
      : AGVCarNumber(AGVCarNumber_),
        robotArmNumber(robotArmNumber_),
        conveyorNumber(conveyorNumber_) {
    this->agvCar = new AGVCar[AGVCarNumber_];
    this->robotArm = new RobotArm[robotArmNumber_];
    this->conveyor = new Conveyor[conveyorNumber_];

    for (int i = 0; i < AGVCarNumber_; i++) {
      agvCar[i].init(AGVCarStatus::arm2);
    }
    for (int i = 0; i < robotArmNumber_; i++) {
      robotArm[i].init(RobotArmStatus::wait);
    }
    for (int i = 0; i < conveyorNumber_; i++) {
      conveyor[i].init(ConveyorStatus::running);
    }
  }

  ~Factory() {
    for (int i = 0; i < this->conveyorNumber; i++) {
      this->conveyor[i].switchConveyor(ConveyorStatus::stopping);
      if (this->conveyor[i].serialIsOpened()) {
        this->conveyor[i].closeSerial();
      }
    }
  }
};
