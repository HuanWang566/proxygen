/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ControlCenterHandler.h"

#include <proxygen/httpserver/RequestHandler.h>
#include <proxygen/httpserver/ResponseBuilder.h>

#include "ControlCenterStats.h"

using namespace proxygen;

DEFINE_bool(request_number,
            true,
            "Include request sequence number in response");

namespace ControlCenterServer {

ControlCenterHandler::ControlCenterHandler(ControlCenterStats* stats) : stats_(stats) {
}

void ControlCenterHandler::onRequest(std::unique_ptr<HTTPMessage> req) noexcept {
  stats_->recordRequest();
  ResponseBuilder builder(downstream_);
  builder.status(200, "OK");

  std::cout << "req cnt, " << stats_->getRequestCount() << std::endl;
  std::cout << "method , " << req->getMethodString() << std::endl;
  std::cout << "DST IP, " << req->getDstIP() << std::endl;
  std::cout << "URL, " << req->getURL() << std::endl;
  std::cout << "path, " << req->getPath() << std::endl;
  std::cout << "query, " << req->getQueryString() << std::endl;
  
  builder.body("hello world");
  if (req->getMethod() == HTTPMethod::GET) {
    std::cout << "this is a http get request" << std::endl;
  }
  if (FLAGS_request_number) {
    builder.header("Request-Number",
                   folly::to<std::string>(stats_->getRequestCount()));
  }
  req->getHeaders().forEach([&](std::string& name, std::string& value) {
    builder.header(folly::to<std::string>("x-ControlCenter-", name), value);
  });
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
