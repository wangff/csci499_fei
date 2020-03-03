#include "keyvaluestore_server.h"

using cs499_fei::KeyValueStoreServiceImpl;

Status KeyValueStoreServiceImpl::put(ServerContext *context,
                                     const PutRequest *request,
                                     PutReply *reply) {
  auto key = request->key();
  auto value = request->value();

  LOG(INFO) << "Received PutRequest. "
            << " Key: " << key;
  threadsafe_map_.Put(key, value);

  return Status::OK;
}

Status KeyValueStoreServiceImpl::get(
    ServerContext *context, ServerReaderWriter<GetReply, GetRequest> *stream) {
  GetRequest request;
  while (stream->Read(&request)) {
    auto key = request.key();
    auto value = threadsafe_map_.Get(key);
    LOG(INFO) << "Received GetRequest. "
              << " Key: " << key;
    GetReply reply;
    if (value.has_value()) {
      reply.set_value(value.value());
    }
    stream->Write(reply);
  }
  return Status::OK;
}

Status KeyValueStoreServiceImpl::remove(ServerContext *context,
                                        const RemoveRequest *request,
                                        RemoveReply *reply) {
  auto key = request->key();
  LOG(INFO) << "Received RemoveRequest. "
            << " Key: " << key;
  threadsafe_map_.Remove(key);
  return Status::OK;
}

// Helper function: to run the gRPC server.
void RunServer() {
  std::string server_address("0.0.0.0:50000");
  KeyValueStoreServiceImpl service;

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

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