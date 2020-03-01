#include "func_service.h"

using cs499_fei::FuncServiceImpl;
using cs499_fei::KeyValueStoreClient;
using cs499_fei::StoragePtr;
using cs499_fei::WarblePtr;
using cs499_fei::WarbleService;

FuncServiceImpl::FuncServiceImpl(StoragePtr storage_ptr, WarblePtr warble_ptr)
    : func_platform_(new FuncPlatform(storage_ptr, warble_ptr)){};

Status FuncServiceImpl::hook(ServerContext *context, const HookRequest *request,
                             HookReply *reply) {
  auto event_type = request->event_type();
  auto event_function = request->event_function();
  LOG(INFO) << "Received HookRequest. "
            << " Key: " << event_type;
  func_platform_->Hook(event_type, event_function);
  return Status::OK;
}

Status FuncServiceImpl::unhook(ServerContext *context,
                               const UnhookRequest *request,
                               UnhookReply *reply) {
  auto event_type = request->event_type();
  LOG(INFO) << "Received HookRequest. "
            << " Key: " << event_type;
  func_platform_->Unhook(event_type);
  return Status::OK;
}

Status FuncServiceImpl::event(ServerContext *context,
                              const EventRequest *request, EventReply *reply) {
  auto event_type = request->event_type();
  auto payload = request->payload();
  LOG(INFO) << "Received EventRequest. "
            << " EventType: " << event_type;
  auto reply_payload_opt = func_platform_->Execute(event_type, payload);
  if (reply_payload_opt.has_value()) {
    reply->mutable_payload()->CopyFrom(reply_payload_opt.value());
    return Status::OK;
  } else {
    return Status::CANCELLED;
  }
}

// Helper function:
// 1. Run the func service grPCC server.
// 2. Create gRPC client to access KeyValue storage.
void RunServer() {
  auto channel = grpc::CreateChannel("localhost:50000",
                                     grpc::InsecureChannelCredentials());
  StoragePtr storage_ptr =
      std::shared_ptr<KeyValueStoreClient>(new KeyValueStoreClient(channel));
  WarblePtr warble_ptr =
      std::shared_ptr<WarbleService>(new WarbleService(storage_ptr));

  std::string server_address("0.0.0.0:50001");
  FuncServiceImpl func_service(storage_ptr, warble_ptr);

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&func_service);

  std::unique_ptr<Server> server(builder.BuildAndStart());

  LOG(INFO) << "Server listening on " << server_address;

  server->Wait();
}

int main(int argc, char **argv) {
  // Initialize Google's logging library.
  google::InitGoogleLogging(argv[0]);

  // Optional: parse command line flags
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  FLAGS_alsologtostderr = 1;

  RunServer();

  return 0;
}