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
using cpp_im_server::DBService;
using cpp_im_server::InsertRequest;
using cpp_im_server::InsertReply;

namespace db_server{
class ServerImpl final {
    std::unique_ptr<ServerCompletionQueue> cq_;
    DBService::AsyncService service_;
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
            CallData(DBService::AsyncService* service, ServerCompletionQueue* cq)
                : service_(service), cq_(cq), responder_(&ctx_), status_(CREATE) {
                Proceed();
            }
            void Proceed();
        private:
            DBService::AsyncService* service_;
            ServerCompletionQueue* cq_;
            ServerContext ctx_;
            InsertRequest request_;
            InsertReply reply_;
            ServerAsyncResponseWriter<InsertReply> responder_;
            enum CallStatus { CREATE, PROCESS, FINISH };
            CallStatus status_;  // The current serving state.
    };
};
}
