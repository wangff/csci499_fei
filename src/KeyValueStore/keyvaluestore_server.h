#ifndef CSCI499_FEI_SRC_KEYVALUESTORE_KEYVALUESTORE_SERVER_H_
#define CSCI499_FEI_SRC_KEYVALUESTORE_KEYVALUESTORE_SERVER_H_

#include <iostream>
#include <string>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <glog/stl_logging.h>
#include <grpcpp/grpcpp.h>

#include "KeyValueStore.grpc.pb.h"
#include "threadsafe_map.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using kvstore::GetReply;
using kvstore::GetRequest;
using kvstore::KeyValueStore;
using kvstore::PutReply;
using kvstore::PutRequest;
using kvstore::RemoveReply;
using kvstore::RemoveRequest;

namespace cs499_fei {
// The implementation of gRPC service KeyValueStore.
// Run as the server to handle gRPC requests for KeyValue Storage.
class KeyValueStoreServiceImpl final : public KeyValueStore::Service {
 public:
  // Receive and process gRPC PutRequest for KeyValue Storage.
  // Put the key-value pair in payload into the storage.
  Status put(ServerContext *context, const PutRequest *request,
             PutReply *reply) override;

  // Receive and process gRPC GetRequest for KeyValue Storage.
  // Get the value from the storage based on the key in the request payload.
  // Construct and return the GetReply with the value.
  Status get(ServerContext *context,
             ServerReaderWriter<GetReply, GetRequest> *stream) override;

  // Receive and process gRPC RemoveRequest for KeyValue Storage.
  // Remove the key-value pair from the storage based ont he key in the request
  // payload.
  Status remove(ServerContext *context, const RemoveRequest *request,
                RemoveReply *reply) override;

 private:
  // Threadsafe hashmap: KeyValue Storage in memory.
  ThreadsafeMap threadsafe_map_;
};
}  // namespace cs499_fei
#endif  // CSCI499_FEI_SRC_KEYVALUESTORE_KEYVALUESTORE_SERVER_H_
