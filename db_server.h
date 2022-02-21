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
                : service_(service), cq_(cq), status_(CREATE) {
                Proceed();
            }
            virtual void Proceed() {
                // std::cout << "CallData Prceed" << std::endl//;
                return;
            }
            enum CallStatus { CREATE, PROCESS, FINISH };
            ServerContext ctx_;
            CallStatus status_;
            ServerCompletionQueue* cq_;
            DBService::AsyncService* service_;
    };

    class InsertOneCallData : public CallData {
        public:
        void Proceed() override {
            doProceed();
        };
        void doProceed();
        InsertOneCallData(DBService::AsyncService* service, ServerCompletionQueue* cq): CallData(service, cq), responder_(&ctx_) {
            // std::cout << "InsertOneCallData Prceed" << std::endl;
            Proceed();
        };
        cpp_im_server::InsertRequest request_;
        cpp_im_server::InsertReply reply_;
        ServerAsyncResponseWriter<cpp_im_server::InsertReply> responder_;
    };

    class FindOneCallData: public CallData {
        public:
        void Proceed() override {
            doProceed();
        };
        void doProceed();
        FindOneCallData(DBService::AsyncService* service, ServerCompletionQueue* cq): CallData(service, cq), responder_(&ctx_) {
            // std::cout << "FindOneCallData Prceed" << std::endl;
            Proceed();
        };
        cpp_im_server::FindRequest request_;    
        cpp_im_server::FindReply reply_;
        ServerAsyncResponseWriter<cpp_im_server::FindReply> responder_;
    };
};


}
