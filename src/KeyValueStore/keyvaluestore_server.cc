#include <iostream>
#include <string>


#include <gflags/gflags.h>
#include <glog/logging.h>
#include <glog/stl_logging.h>
#include <grpcpp/grpcpp.h>

#include "threadsafe_map.h"
#include "../build/KeyValueStore.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using kvstore::PutRequest;
using kvstore::PutReply;
using kvstore::GetRequest;
using kvstore::GetReply;
using kvstore::RemoveRequest;
using kvstore::RemoveReply;
using kvstore::KeyValueStore;

class KeyValueStoreServiceImpl final : public KeyValueStore::Service {
 public:
  Status put(ServerContext *context, const PutRequest *request, PutReply *reply) override {

    auto key = request->key();
    auto value = request->value();

    LOG(INFO) << "Received PutRequest. "
              << " Key: " << key;
    threadsafe_map_.Put(key, value);

    return Status::OK;
  }

  Status get(ServerContext *context, ServerReaderWriter<GetReply, GetRequest> *stream) override {
    GetRequest request;
    while (stream->Read(&request)) {
      auto key = request.key();
      auto value = threadsafe_map_.Get(key);
      LOG(INFO) << "Received GetRequest. "
                << " Key: " << key;
      GetReply reply;
      if(value.has_value()){
        reply.set_value(value.value());
      }
      stream->Write(reply);
    }
    return Status::OK;
  }

  Status remove(ServerContext *context, const RemoveRequest *request,
                RemoveReply *reply) override {
    auto key = request->key();
    LOG(INFO) << "Received RemoveRequest. "
              << " Key: " << key;
    threadsafe_map_.Remove(key);
    return Status::OK;
  }

 private:
  ThreadsafeMap threadsafe_map_;
};

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

  RunServer();
  return 0;
}