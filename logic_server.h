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
using grpc::CompletionQueue;
using grpc::ServerContext;
using grpc::ClientContext;
using grpc::ClientAsyncResponseReader;
using grpc::Status;
using cpp_im_server::LogicService;
using cpp_im_server::DBService;

namespace logic_server{
    class ServerImpl final {
        std::unique_ptr<ServerCompletionQueue> cq_;
        LogicService::AsyncService service_;
        std::unique_ptr<Server> server_;
        std::unique_ptr<DBService::Stub> db_stub;
        CompletionQueue db_cq;

        void HandleRpcs();
        void tick();
        void tryInsert();
        public:
            ~ServerImpl()   {
                server_->Shutdown();
                cq_->Shutdown();
            }
            void Run(int port);
    };

    struct AsyncClientCall {
        cpp_im_server::InsertReply reply;
        ClientContext context;
        Status status;
        std::unique_ptr<ClientAsyncResponseReader<cpp_im_server::InsertReply>> response_reader;
    };
}
