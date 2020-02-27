#include "func_service_client.h"
namespace cs499_fei {
FuncServiceClient::FuncServiceClient(std::shared_ptr<grpc::Channel> channel)
    : stub_(FuncService::NewStub(channel)){};

// Send the hooking gRPC requests to register the mapping relationship between
// event_type and event_function
void FuncServiceClient::Hook(const int event_type,
                             const std::string &event_function) {
  // Data we are sending to the server.
  HookRequest request;
  request.set_event_type(event_type);
  request.set_event_function(event_function);

  // Container for the data we expect from the server.
  HookReply reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  grpc::ClientContext context;

  // The actual RPC.
  Status status = stub_->hook(&context, request, &reply);

  // Act upon its status.
  if (status.ok()) {
    LOG(INFO) << "HooKRequest RPC succeed, EventType: " << event_type;
  } else {
    // Fail to put key-value pair.
    LOG(ERROR) << "HooKRequest RPC failed, Key: " << event_type << std::endl
               << "Error: " << status.error_code() << ": "
               << status.error_message();
  }
}

// Send the unhooking gRPC requests to remove the mapping relationship based on
// event_type
void FuncServiceClient::UnHook(const int event_type) {
  // Data we are sending to the server.
  UnhookRequest request;
  request.set_event_type(event_type);

  // Container for the data we expect from the server.
  UnhookReply reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  grpc::ClientContext context;

  // The actual RPC.
  Status status = stub_->unhook(&context, request, &reply);

  // Act upon its status.
  if (status.ok()) {
    LOG(INFO) << "UnhookRequest RPC succeed, EventType: " << event_type;
  } else {
    // Fail to put key-value pair.
    LOG(ERROR) << "UnhookRequest RPC failed, Key: " << event_type << std::endl
               << "Error: " << status.error_code() << ": "
               << status.error_message();
  }
}

// Send the event gRPC requests to execute the specified warble handler function
Any FuncServiceClient::Event(const int event_type, Any *payload) {
  // Data we are sending to the server.
  EventRequest request;
  request.set_event_type(event_type);
  request.set_allocated_payload(payload);

  // Container for the data we expect from the server.
  EventReply reply;

  // Context for the client. It could be used to convey extra information to
  // the server and/or tweak certain RPC behaviors.
  grpc::ClientContext context;

  // The actual RPC.
  Status status = stub_->event(&context, request, &reply);

  // Act upon its status.
  if (status.ok()) {
    LOG(INFO) << "UnhookRequest RPC succeed, EventType: " << event_type;
  } else {
    // Fail to put key-value pair.
    LOG(ERROR) << "UnhookRequest RPC failed, Key: " << event_type << std::endl
               << "Error: " << status.error_code() << ": "
               << status.error_message();
  }
  return reply.payload();
}
}  // namespace cs499_fei