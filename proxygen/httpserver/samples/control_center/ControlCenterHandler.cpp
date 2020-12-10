/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include "ControlCenterHandler.h"
#include <string>

#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/ResponseBuilder.h>

#include "ControlCenterStats.h"
#include "factory_demo/factory.h"
#include "factory_demo/utils.h"

using namespace proxygen;

DEFINE_bool(request_number,
            true,
            "Include request sequence number in response");

namespace ControlCenterServer {

// TODO:check why must define this varible in this file
Factory my_factory(2, 3, 1);

ControlCenterHandler::ControlCenterHandler(ControlCenterStats* stats)
    : stats_(stats) {
}

void ControlCenterHandler::onRequest(
    std::unique_ptr<HTTPMessage> req) noexcept {
  stats_->recordRequest();
  ResponseBuilder builder(downstream_);
  builder.status(200, "OK");

  // std::cout << "req cnt, " << stats_->getRequestCount() << std::endl;
  // std::cout << "method , " << req->getMethodString() << std::endl;
  // std::cout << "DST IP, " << req->getDstIP() << std::endl;
  std::cout << "URL, " << req->getURL() << std::endl;
  std::cout << "path, " << req->getPath() << std::endl;
  std::cout << "query, " << req->getQueryString() << std::endl;
  std::cout << "--------------------------------------" << std::endl;

  // builder.body("hello world");
  // if (req->getMethod() == HTTPMethod::GET) {
  //   std::cout << "this is a http get request" << std::endl;
  // }

  std::string path = req->getPath();
  int defVal = 0;

  // TODO: when set a value, use httpPut instead of  httpGet

  if (!path.compare(GET_ROBOT_ARM_STATUS_URL_PATH)) {
    int arm_id = req->getIntQueryParam("arm_id", defVal);
    // my_factory.findRobotArmByID(arm_id)->getStatusToString();
    builder.body(my_factory.findRobotArmByID(arm_id)->getStatusToString());
  } else if (!path.compare(GET_AGV_CAR_STATUS_URL_PATH)) {
    int car_id = req->getIntQueryParam("car_id", defVal);
    builder.body(my_factory.findAGVCarByID(car_id)->getStatusToString());
  } else if (!path.compare(GET_CONVEYOR_STATUS_URL_PATH)) {
    int conveyor_id = req->getIntQueryParam("conveyor_id", defVal);
    builder.body(my_factory.findConveyorByID(conveyor_id)->getStatusToString());
  } else if (!path.compare(GET_BLOCK_COLOR_URL_PATH)) {
    int arm_id = req->getIntQueryParam("arm_id", defVal);
    builder.body(my_factory.findRobotArmByID(arm_id)->getBlockColorToString());
  } else if (!path.compare(SET_ROBOT_ARM_STATUS_URL_PATH)) {
    int arm_id = req->getIntQueryParam("arm_id", defVal);
    string status = req->getQueryParam("status");
    if (my_factory.findRobotArmByID(arm_id)->setStatus(status)) {
      builder.body("SUCCESS");
    } else {
      builder.body("FAIL");
    }
  } else if (!path.compare(SET_AGV_CAR_STATUS_URL_PATH)) {
    int car_id = req->getIntQueryParam("car_id");
    string status = req->getQueryParam("status");
    if (my_factory.findAGVCarByID(car_id)->setStatus(status)) {
      builder.body("SUCCESS");
    } else {
      builder.body("FAIL");
    }
  } else if (!path.compare(SET_CONVEYOR_STATUS_URL_PATH)) {
    int conveyor_id = req->getIntQueryParam("conveyor_id");
    string status = req->getQueryParam("status");
    if (my_factory.findConveyorByID(conveyor_id)->setStatus(status)) {
      builder.body("SUCCESS");
    } else {
      builder.body("FAIL");
    }
  } else if (!path.compare(SET_BLOCK_COLOR_URL_PATH)) {
    int arm_id = req->getIntQueryParam("arm_id");
    string color = req->getQueryParam("color");
    if (my_factory.findRobotArmByID(arm_id)->setBlockColor(color)) {
      builder.body("SUCCESS");
    } else {
      builder.body("FAIL");
    }
  } else if (!path.compare(GET_SET_ROBOT_ARM_NOT_STATUS_URL_PATH)) {
    int arm_id = req->getIntQueryParam("arm_id", defVal);
    string not_status = req->getQueryParam("not_status");
    string status = req->getQueryParam("status");
    if (my_factory.findRobotArmByID(arm_id)->getSetNotStatus(not_status,
                                                             status)) {
      builder.body("SUCCESS");
    } else {
      builder.body("FAIL");
    }
  } else if (!path.compare(GET_ROBOT_ARM_IS_STATUS_URL_PATH)) {
    int arm_id = req->getIntQueryParam("arm_id", defVal);
    string is_status = req->getQueryParam("is_status");
    string status = req->getQueryParam("status");
    if (my_factory.findRobotArmByID(arm_id)->getSetIsStatus(is_status,
                                                            status)) {
      builder.body("SUCCESS");
    } else {
      builder.body("FAIL");
    }
  }

  if (req->hasQueryParam("account")) {
    std::cout << "communicate successes" << std::endl;
  }
  // if (FLAGS_request_number) {
  //   builder.header("Request-Number",
  //                  folly::to<std::string>(stats_->getRequestCount()));
  // }
  // req->getHeaders().forEach([&](std::string& name, std::string& value) {
  //   builder.header(folly::to<std::string>("x-ControlCenter-", name),
  //   value);
  // });
  builder.send();
}

void ControlCenterHandler::onBody(std::unique_ptr<folly::IOBuf> body) noexcept {
  ResponseBuilder(downstream_).body(std::move(body)).send();
}

void ControlCenterHandler::onEOM() noexcept {
  ResponseBuilder(downstream_).sendWithEOM();
}

void ControlCenterHandler::onUpgrade(UpgradeProtocol /*protocol*/) noexcept {
  // handler doesn't support upgrades
}

void ControlCenterHandler::requestComplete() noexcept {
  delete this;
}

void ControlCenterHandler::onError(ProxygenError /*err*/) noexcept {
  delete this;
}
} // namespace ControlCenterServer
