#ifndef FAAS_SRC_FUNC_KEYVALUESTORE_CLIENT_H_
#define FAAS_SRC_FUNC_KEYVALUESTORE_CLIENT_H_

#include "storage_abstraction.h"

#include <grpcpp/grpcpp.h>
#include <grpcpp/channel.h>

#include "KeyValueStore.grpc.pb.h"

using grpc::Channel;
using kvstore::KeyValueStore;

class KeyValueStoreClient : public StorageAbstraction {
 public:
  explicit KeyValueStoreClient(std::shared_ptr<grpc::Channel>);

  void Put(const std::string &, const std::string &) override;

  // Get values based on keys
  StringOptionalVector Get(const StringVector &) override;

  // Remove a value based on a key
  void Remove(const std::string &) override;

 private:
  std::unique_ptr<KeyValueStore::Stub> stub_{};
};

#endif //FAAS_SRC_FUNC_KEYVALUESTORE_CLIENT_H_
