#include <iostream>
#include <memory>
#include <string>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>
#include "cpp_im_server.grpc.pb.h"
#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

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
    mongocxx::instance instance{};
    mongocxx::uri uri_;
    mongocxx::client client_;
    void HandleRpcs();
    public:
        ServerImpl() {
            uri_ = mongocxx::uri("mongodb://192.168.2.142:27017");
            client_ = mongocxx::client(uri_);
        }
        ~ServerImpl()   {
            server_->Shutdown();
            cq_->Shutdown();
        }
        void Run(int port);

    class CallData {
        public:
            CallData(DBService::AsyncService* service, ServerCompletionQueue* cq, mongocxx::client* client)
                : service_(service), cq_(cq), status_(CREATE) , client_(client){
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
            mongocxx::client* client_;
    };

    class InsertOneCallData : public CallData {
        public:
        void Proceed() override {
            doProceed();
        };
        void doProceed();
        InsertOneCallData(DBService::AsyncService* service, ServerCompletionQueue* cq, mongocxx::client* client): CallData(service, cq, client), responder_(&ctx_) {
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
        FindOneCallData(DBService::AsyncService* service, ServerCompletionQueue* cq, mongocxx::client* client): CallData(service, cq, client), responder_(&ctx_) {
            // std::cout << "FindOneCallData Prceed" << std::endl;
            Proceed();
        };
        cpp_im_server::FindRequest request_;    
        cpp_im_server::FindReply reply_;
        ServerAsyncResponseWriter<cpp_im_server::FindReply> responder_;
    };
};


}
