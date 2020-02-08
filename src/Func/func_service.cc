#include "func_service.h"

FuncServiceImpl::FuncServiceImpl(StoragePtr storage_ptr, WarblePtr warble_ptr): func_platform_(new FuncPlatform(storage_ptr,warble_ptr)) {};

Status FuncServiceImpl::hook(ServerContext *context, const HookRequest *request, HookReply *reply) {
  auto event_type = request->event_type();
  auto event_function = request->event_function();
  LOG(INFO) << "Received HookRequest. "
            << " Key: " << event_type;
  func_platform_->Hook(event_type, event_function);
  return Status::OK;
}

Status FuncServiceImpl::unhook(ServerContext *context, const UnhookRequest *request, UnhookReply *reply) {
  auto event_type = request->event_type();
  LOG(INFO) << "Received HookRequest. "
            << " Key: " << event_type;
  func_platform_ -> Unhook(event_type);
  return Status::OK;
}

Status FuncServiceImpl::event(ServerContext *context, const EventRequest *request, EventReply *reply) {
  return Status::OK;
}

// Helper function:
// 1. Run the func service grPCC server.
// 2. Create gRPC client to access KeyValue storage.
void RunServer() {
  auto channel = grpc::CreateChannel("localhost:50000", grpc::InsecureChannelCredentials());
  StoragePtr storage_ptr = std::shared_ptr<KeyValueStoreClient>(new KeyValueStoreClient(channel));
  WarblePtr warble_ptr = std::shared_ptr<Warble>(new Warble);

  std::string server_address("0.0.0.0:50001");
  FuncServiceImpl func_service(storage_ptr,warble_ptr);

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

  RunServer();

  return 0;
}