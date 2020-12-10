#pragma once
#ifndef __FACTRORY_H__
#define __FACTRORY_H__

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

enum BlockColor { none, red, green };

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

  string getStatusToString() {
    switch (this->status) {
      case AGVCarStatus::arm2:
        return "arm2";
        break;
      case AGVCarStatus::arm3:
        return "arm3";
        break;
      case AGVCarStatus::moving:
        return "moving";
        break;
      default:
        return "error";
        break;
    }
  }

  bool setStatus(string status_) {
    AGVCarStatus status_temp;
    if (transferStringStatus(status_, status_temp)){
      setStatus(status_temp);
      return true;
    }
    return false;
  }

  bool transferStringStatus(std::string statusString_, AGVCarStatus& status_) {
    if (!statusString_.compare("arm2")) {
      status_ = AGVCarStatus::arm2;
      return true;
    }
    if (!statusString_.compare("arm3")) {
      status_ = AGVCarStatus::arm3;
      return true;
    }
    if (!statusString_.compare("moving")) {
      status_ = AGVCarStatus::moving;
      return true;
    }

    return false;
  }
};

class RobotArm {
 private:
  RobotArmStatus status;
  BlockColor blockColor = BlockColor::none;

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

  bool setStatus(string status_) {
    RobotArmStatus status_temp;
    if (transferStringStatus(status_, status_temp)){
      setStatus(status_temp);
      return true;
    }
    return false;
  }

  bool transferStringStatus(std::string statusString_,
                            RobotArmStatus& status_) {
    if (!statusString_.compare("wait")) {
      status_ = RobotArmStatus::wait;
      return true;
    }
    if (!statusString_.compare("ready")) {
      status_ = RobotArmStatus::ready;
      return true;
    }
    if (!statusString_.compare("startPick")) {
      status_ = RobotArmStatus::startPick;
      return true;
    }
    if (!statusString_.compare("finish")) {
      status_ = RobotArmStatus::finish;
      return true;
    }

    return false;
  }

  bool setBlockColor(string color_) {
    BlockColor color_temp;
    if (transferStringBlockColor(color_, color_temp)){
      setBlockColor(color_temp);
      return true;
    }
    return false;
  }

  bool transferStringBlockColor(std::string colorString_, BlockColor& color_) {
    if (!colorString_.compare("none")) {
      color_ = BlockColor::none;
      return true;
    }
    if (!colorString_.compare("red")) {
      color_ = BlockColor::red;
      return true;
    }
    if (!colorString_.compare("green")) {
      color_ = BlockColor::green;
      return true;
    }
    return false;
  }

  BlockColor getBlockColor() {
    return this->blockColor;
  }

  void setBlockColor(BlockColor color_) {
    this->blockColor = color_;
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

  bool getSetIsStatus(string isStatus_, string status_) {
    RobotArmStatus is_status_temp;
    RobotArmStatus status_temp;
    if (transferStringStatus(status_, status_temp) &&
        transferStringStatus(isStatus_, is_status_temp)) {
      if (getSetIsStatus(is_status_temp, status_temp)) {
        return true;
      }
    }
    return false;
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

  bool getSetNotStatus(string notStatus_, string status_) {
    RobotArmStatus not_status_temp;
    RobotArmStatus status_temp;
    if (transferStringStatus(status_, status_temp) &&
        transferStringStatus(notStatus_, not_status_temp)) {
      if (getSetNotStatus(not_status_temp, status_temp)) {
        return true;
      }
    }
    return false;
  }

  string getStatusToString() {
    switch (this->status) {
      case RobotArmStatus::wait:
        return "wait";
        break;
      case RobotArmStatus::ready:
        return "ready";
        break;
      case RobotArmStatus::startPick:
        return "startPick";
        break;
      case RobotArmStatus::finish:
        return "finish";
        break;
      default:
        return "error";
        break;
    }
  }

  string getBlockColorToString() {
    switch (this->blockColor) {
      case BlockColor::none:
        return "none";
        break;
      case BlockColor::red:
        return "red";
        break;
      case BlockColor::green:
        return "green";
        break;
      default:
        return "error";
        break;
    }
  }
};

class Conveyor {
 private:
  ConveyorStatus status;
  CSerialPort m_serialport;
  std::string portName = "/dev/ttyS5";
  // std::string portName = "/dev/pts/0";
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

  string getStatusToString() {
    switch (this->status) {
      case ConveyorStatus::running:
        return "running";
        break;
      case ConveyorStatus::stopping:
        return "stopping";
        break;
      default:
        return "error";
        break;
    }
  }

  bool setStatus(string status_) {
    ConveyorStatus status_temp;
    if (transferStringStatus(status_, status_temp)){
      setStatus(status_temp);
      return true;
    }
    return false;
  }

  bool transferStringStatus(std::string statusString_,
                            ConveyorStatus& status_) {
    if (!statusString_.compare("run")) {
      status_ = ConveyorStatus::running;
      return true;
    }
    if (!statusString_.compare("stop")) {
      status_ = ConveyorStatus::stopping;
      return true;
    }

    return false;
  }
};

class Factory {
 private:
  int AGVCarNumber;
  int robotArmNumber;
  int conveyorNumber;
  RobotArm* robotArm;
  AGVCar* agvCar;
  Conveyor* conveyor;

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

  RobotArm* findRobotArmByID(int id) {
    assert(id >= 0 && id < this->robotArmNumber);
    return &robotArm[id - 1];
  }

  AGVCar* findAGVCarByID(int id) {
    assert(id >= 0 && id < this->AGVCarNumber);
    return &agvCar[id - 1];
  }

  Conveyor* findConveyorByID(int id) {
    assert(id >= 0 && id < this->conveyorNumber);
    return &conveyor[id - 1];
  }
};

#endif