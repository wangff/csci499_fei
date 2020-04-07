#include "keyvaluestore_server.h"

using cs499_fei::KeyValueStoreServiceImpl;
using cs499_fei::Persistence;
using cs499_fei::PersistenceAbstraction;
using cs499_fei::PersistPtr;

using cs499_fei::FLAGS_store;

KeyValueStoreServiceImpl::KeyValueStoreServiceImpl() {
  bool flag_store_not_set =
      gflags::GetCommandLineFlagInfoOrDie("store").is_default;

  if (flag_store_not_set) {
    LOG(INFO) << "In-memory model." << std::endl;
  } else {
    LOG(INFO) << "Persistence model." << std::endl;
    LOG(INFO) << "Persistence location: " << FLAGS_store << std::endl;
    PersistPtr persist_ptr = std::shared_ptr<Persistence>(new Persistence());
    threadsafe_map_ = ThreadsafeMap(persist_ptr, FLAGS_store);
  }
}

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

void KeyValueStoreServiceImpl::store() { threadsafe_map_.Store(FLAGS_store); }

// Use unnamed namespace to make static functions in it.
// Define two methods to bridge signal handler to the customized function
namespace {
std::function<void(int)> shutdownHandler;
void signalHandler(int signal) { shutdownHandler(signal); }
}  // namespace

// Helper function: to run the gRPC server.
void RunServer() {
  std::string server_address("0.0.0.0:50000");

  KeyValueStoreServiceImpl service;

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  std::unique_ptr<Server> server(builder.BuildAndStart());

  LOG(INFO) << "Server listening on " << server_address;

  // register signal SIGINT and signal handler
  signal(SIGINT, signalHandler);
  shutdownHandler = [&](int signal) {
    std::cout << "Server shutdown... " <<std::endl;
    service.store();
    exit(signal);
  };

  server->Wait();
}

int main(int argc, char **argv) {
  // Initialize Google's logging library.
  google::InitGoogleLogging(argv[0]);

  // Parse command line flags
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  FLAGS_alsologtostderr = 1;

  RunServer();
  return 0;
}