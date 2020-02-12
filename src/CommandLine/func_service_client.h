#ifndef CSCI499_FEI_SRC_FRONTEND_FUNC_SERVICE_CLIENT_H_
#define CSCI499_FEI_SRC_FRONTEND_FUNC_SERVICE_CLIENT_H_
#include <iostream>
#include <optional>
#include <string>

#include <glog/logging.h>
#include <grpcpp/grpcpp.h>

#include "Func.grpc.pb.h"

using google::protobuf::Any;
using google::protobuf::Message;
using grpc::Status;
using func::HookRequest;
using func::HookReply;
using func::UnhookRequest;
using func::UnhookReply;
using func::EventRequest;
using func::EventReply;
using func::FuncService;

// The client of gRPC to send requests to FuncService
class FuncServiceClient {
 public:
  // Constructor with the argument of shared pointer of Channel
  FuncServiceClient(std::shared_ptr<grpc::Channel>);

  // Send the hooking gRPC requests to register the mapping relationship between event_type and event_function
  void Hook(const int event_type, const std::string &event_function);

  // Send the unhooking gRPC requests to remove the mapping relationship based on event_type
  void UnHook(const int event_type);

  // Send the event gRPC requests to execute the specified warble handler function
  Any Event(const int event_type, Any *payload);

 private:
  std::unique_ptr<FuncService::Stub> stub_;
};
#endif //CSCI499_FEI_SRC_FRONTEND_FUNC_SERVICE_CLIENT_H_
