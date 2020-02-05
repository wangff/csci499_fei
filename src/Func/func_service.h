#ifndef CSCI499_FEI_SRC_FUNC_FUNC_SERVICE_H_
#define CSCI499_FEI_SRC_FUNC_FUNC_SERVICE_H_

#include <iostream>
#include <string>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <grpcpp/grpcpp.h>

#include "keyvaluestore_client.h"
#include "Func.grpc.pb.h"
#include "func_platform.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using func::HookRequest;
using func::HookReply;
using func::UnhookRequest;
using func::UnhookReply;
using func::EventRequest;
using func::EventReply;
using func::FuncService;

// The implementation of gRPC service FuncService.
// Run as the server to handle gRPC requests for Func.
class FuncServiceImpl final : public FuncService::Service {
 public:
  // Constructor with the argument StoragePtr that is used to access the KeyValue storage.
  FuncServiceImpl(StoragePtr storage_ptr, WarblePtr warble_ptr);

  // Receive and process gRPC HookRequest for Func.
  // Hook the pair of event type and event function.
  Status hook(ServerContext *context, const HookRequest *request, HookReply *reply) override;

  // Receive and process gRPC UnHookRequest for Func.
  // Unhook the pair of event type and event function, if it has been hooked.
  Status unhook(ServerContext *context, const UnhookRequest *request, UnhookReply *reply) override;

  // Receive and process gRPC EventRequest for Func.
  // Execute the corresponding handler function based on event type.
  Status event(ServerContext *context, const EventRequest *request, EventReply *reply) override;

 private:
  // Pointer of func platform.
  std::unique_ptr<FuncPlatform> func_platform_;
};

#endif //CSCI499_FEI_SRC_FUNC_FUNC_SERVICE_H_
