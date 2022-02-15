#include <iostream>
#include <memory>
#include <string>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>
#include "cpp_im_server.grpc.pb.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;
using cpp_im_server::HeartBeat;
using cpp_im_server::HeartBeatRequest;
using cpp_im_server::HeartBeatReply;

namespace db_server{
class ServerImpl final {
    std::unique_ptr<ServerCompletionQueue> cq_;
    HeartBeat::AsyncService service_;
    std::unique_ptr<Server> server_;
    void HandleRpcs();
    public:
        ~ServerImpl()   {
            server_->Shutdown();
            cq_->Shutdown();
        }
        void Run(int port);
    
    class CallData {
        public:
            CallData(HeartBeat::AsyncService* service, ServerCompletionQueue* cq)
                : service_(service), cq_(cq), responder_(&ctx_), status_(CREATE) {
                Proceed();
            }
            void Proceed();
        private:
            HeartBeat::AsyncService* service_;
            ServerCompletionQueue* cq_;
            ServerContext ctx_;
            HeartBeatRequest request_;
            HeartBeatReply reply_;
            ServerAsyncResponseWriter<HeartBeatReply> responder_;
            enum CallStatus { CREATE, PROCESS, FINISH };
            CallStatus status_;  // The current serving state.
    };
};
}
