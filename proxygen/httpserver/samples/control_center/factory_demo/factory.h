#include <atomic>
#include <iostream>

enum AGVCarStatus { arm2, arm3, moving };

enum RobotArmStatus { wait, ready, startPick, finish };

enum ConveyorStatus { running, stopping };

class AGVCar {
 private:
  AGVCarStatus status;

 public:
  AGVCar();

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
  RobotArm();

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
      std::cout << "swithc on conveyor" << std::endl;
      return true;
    } else if (status == ConveyorStatus::stopping) {
      std::cout << "swithc off conveyor" << std::endl;
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
  }
};
