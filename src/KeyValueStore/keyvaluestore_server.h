#ifndef CSCI499_FEI_SRC_KEYVALUESTORE_KEYVALUESTORE_SERVER_H_
#define CSCI499_FEI_SRC_KEYVALUESTORE_KEYVALUESTORE_SERVER_H_

#include <csignal>
#include <iostream>
#include <string>

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <glog/stl_logging.h>
#include <grpcpp/grpcpp.h>

#include "KeyValueStore.grpc.pb.h"
#include "persistence_abstraction.h"
#include "persistence.h"
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
// Define the flag for the storage commandline
DEFINE_string(store, "data_file",
              "Store the in-memory data in the specified file.");

// The implementation of gRPC service KeyValueStore.
// Run as the server to handle gRPC requests for KeyValue Storage.
class KeyValueStoreServiceImpl final : public KeyValueStore::Service {
 public:
  // KeyValueStoreServiceImpl default constructor
  KeyValueStoreServiceImpl();
//
//  // KeyValueStoreServiceImpl constructor with the parameter of thread_safe_map
//  KeyValueStoreServiceImpl(const PersistPtr &persist_ptr,
//                           const std::string &file_name)
//      : threadsafe_map_(ThreadsafeMap(persist_ptr, file_name)){};

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

  // Store the in-memory data into the file.
  void store();

 private:
  // Threadsafe hashmap: KeyValue Storage in memory.
  ThreadsafeMap threadsafe_map_;
};
}  // namespace cs499_fei
#endif  // CSCI499_FEI_SRC_KEYVALUESTORE_KEYVALUESTORE_SERVER_H_
