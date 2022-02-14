#include <iostream>
#include <memory>
#include <string>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>
#include "logic_server.grpc.pb.h"
using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;
using cpp_im_server::HeartBeat;
using cpp_im_server::HeartBeatRequest;
using cpp_im_server::HeartBeatReply;


class ServerImpl final {
    std::unique_ptr<ServerCompletionQueue> cq_;
    HeartBeat::AsyncService service_;
    std::unique_ptr<Server> server_;

    class CallData {
        public:
        // Take in the "service" instance (in this case representing an asynchronous
        // server) and the completion queue "cq" used for asynchronous communication
        // with the gRPC runtime.
            CallData(HeartBeat::AsyncService* service, ServerCompletionQueue* cq)
                : service_(service), cq_(cq), responder_(&ctx_), status_(CREATE) {
            // Invoke the serving logic right away.
                Proceed();
            }
            
            void Proceed() {
                if (status_ == CREATE) {
                    // Make this instance progress to the PROCESS state.
                    status_ = PROCESS;

                    // As part of the initial CREATE state, we *request* that the system
                    // start processing SayHello requests. In this request, "this" acts are
                    // the tag uniquely identifying the request (so that different CallData
                    // instances can serve different requests concurrently), in this case
                    // the memory address of this CallData instance.
                    service_->Requestcheck_heart_beat(&ctx_, &request_, &responder_, cq_, cq_, this);
                } else if (status_ == PROCESS) {
                    // Spawn a new CallData instance to serve new clients while we process
                    // the one for this CallData. The instance will deallocate itself as
                    // part of its FINISH state.
                    new CallData(service_, cq_);

                    // The actual processing.
                    std::string prefix("Hello ");
                    reply_.set_message(prefix + request_.name());

                    // And we are done! Let the gRPC runtime know we've finished, using the
                    // memory address of this instance as the uniquely identifying tag for
                    // the event.
                    status_ = FINISH;
                    responder_.Finish(reply_, Status::OK, this);
                } else {
                    GPR_ASSERT(status_ == FINISH);
                    // Once in the FINISH state, deallocate ourselves (CallData).
                    delete this;
                }
            }
        private:
            // The means of communication with the gRPC runtime for an asynchronous
            // server.
            HeartBeat::AsyncService* service_;
            // The producer-consumer queue where for asynchronous server notifications.
            ServerCompletionQueue* cq_;
            // Context for the rpc, allowing to tweak aspects of it such as the use
            // of compression, authentication, as well as to send metadata back to the
            // client.
            ServerContext ctx_;
            // What we get from the client.
            HeartBeatRequest request_;
            // What we send back to the client.
            HeartBeatReply reply_;
            // The means to get back to the client.
            ServerAsyncResponseWriter<HeartBeatReply> responder_;
            // Let's implement a tiny state machine with the following states.
            enum CallStatus { CREATE, PROCESS, FINISH };
            CallStatus status_;  // The current serving state.
    };

    public:
    ~ServerImpl() {
        server_->Shutdown();
        // Always shutdown the completion queue after the server.
        cq_->Shutdown();
    }

};
