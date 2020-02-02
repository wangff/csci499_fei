#include "keyvaluestore_client.h"

#include <iostream>
#include <optional>

#include <glog/logging.h>
#include <grpcpp/grpcpp.h>

using grpc::Status;
using kvstore::PutRequest;
using kvstore::PutReply;
using kvstore::GetRequest;
using kvstore::GetReply;
using kvstore::RemoveRequest;
using kvstore::RemoveReply;
using kvstore::KeyValueStore;

KeyValueStoreClient::KeyValueStoreClient(std::shared_ptr<grpc::Channel> channel)
    : stub_(KeyValueStore::NewStub(channel)) {}

void KeyValueStoreClient::Put(const std::string &key, const std::string &value) {
  PutRequest request;
  request.set_key(key);
  request.set_value(value);

  PutReply reply;

  grpc::ClientContext context;

  Status status = stub_->put(&context, request, &reply);
  if (status.ok()) {
    LOG(INFO) << "PutRequest RPC succeed, Key: " << key;
  } else {
    // Fail to put key-value pair.
    LOG(ERROR) << "PutRequest RPC failed, Key: " << key << std::endl
               << "Error: " << status.error_code() << ": " << status.error_message();
  }
}

/// Given a series of keys, request their values from server.
StringOptionalVector KeyValueStoreClient::Get(const StringVector &key_vector) {
  StringOptionalVector value_vector;
  grpc::ClientContext context;

  auto stream = stub_->get(&context);
  for (const auto &key : key_vector) {
    GetRequest request;
    request.set_key(key);
    if (stream->Write(request)) {
      GetReply reply;
      stream->Read(&reply);
      auto value = reply.value();
      value_vector.push_back(StringOptional{value});
    } else {
      // stream write failed.
      value_vector.push_back(StringOptional());
    }
  }

  stream->WritesDone();

  Status status = stream->Finish();
  if (status.ok()) {
    LOG(INFO) << "GetRequest RPC succeed";
  } else {
    LOG(ERROR) << "GetRequest RPC failed"
               << "Error: " << status.error_code() << ", " << status.error_message();
  }
  return value_vector;
}

void KeyValueStoreClient::Remove(const std::string &key) {
  RemoveRequest request;
  request.set_key(key);

  RemoveReply reply;
  grpc::ClientContext context;
  Status status = stub_->remove(&context, request, &reply);

  if (status.ok()) {
    LOG(INFO) << "RemoveRequest RPC succeed, Key: " << key;
  } else {
    LOG(ERROR) << "RemoveRequest RPC failed"
               << "Error: " << status.error_code() << ", " << status.error_message();
  }
}

int main(int argc, char **argv) {
  // Initialize Google's logging library.
  google::InitGoogleLogging(argv[0]);

  // Optional: parse command line flags
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  auto channel = grpc::CreateChannel("localhost:50000", grpc::InsecureChannelCredentials());
  KeyValueStoreClient kv_store_client(channel);

  std::vector<std::string> key_vector = {"first", "second", "third", "forth"};
  for (auto key:key_vector) {
    kv_store_client.Put(key, key + " value");
  }

  auto value_vector = kv_store_client.Get(key_vector);

  // remove all key-value pairs.
  for (auto key:key_vector) {
    kv_store_client.Remove(key);
  }

  value_vector = kv_store_client.Get(key_vector);
  return 0;
}