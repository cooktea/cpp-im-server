#include <iostream>
#include <memory>
#include <string>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>
#include "cpp_im_server.grpc.pb.h"
#include "logic_server.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;
using cpp_im_server::LogicService;

void logic_server::ServerImpl::HandleRpcs() {
}

void logic_server::ServerImpl::tick() {
    if (rand() % 100 > 50){
        tryInsert();
    }
    else{
        findOne();
    }
    void* got_tag;
    bool ok = false;
    bool res = db_cq.Next(&got_tag, &ok);
    if (res) {
        AsyncClientCall* call = static_cast<AsyncClientCall*>(got_tag);
        GPR_ASSERT(ok);
        call->Proceed();
        if (call->status.ok())
            std::cout << "RPC Call Completed. "  << std::endl;
        else
            std::cout << "RPC failed" << std::endl;
        delete call;
    }
}

void logic_server::ServerImpl::findOne() {
    cpp_im_server::FindRequest request;
    request.set_db_name("cpp-im-dev");
    request.set_col_name("user");
    request.set_query("{'user_name': 'chenkang'}");
    std::cout << "findOne" << std::endl;
    AsyncClientFindOneCall* call = new AsyncClientFindOneCall;
    call->response_reader = db_stub->PrepareAsyncfind_one(&call->context, request, &db_cq);
    call->response_reader->StartCall();
    call->response_reader->Finish(&call->reply, &call->status, (void*)call);
}

void logic_server::ServerImpl::tryInsert() {
    cpp_im_server::InsertRequest request;
    request.set_db_name("cpp-im-dev");
    request.set_col_name("user");
    std::string doc = "{'name':'" + std::to_string(rand()) + "'}";
    request.set_doc(doc);
    std::cout << "tryInsert" << std::endl;
    AsyncClientInsertCall* call = new AsyncClientInsertCall;
    call->response_reader = db_stub->PrepareAsyncinsert_one(&call->context, request, &db_cq);
    call->response_reader->StartCall();
    call->response_reader->Finish(&call->reply, &call->status, (void*)call);
    std::cout << "tryInsert end" << std::endl;

}

void logic_server::ServerImpl::Run(int port) {
    sleep(rand() % 5);
    std::string server_address = std::string("0.0.0.0:") + std::to_string(port);
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service_);
    cq_ = builder.AddCompletionQueue();
    server_ = builder.BuildAndStart();
    std::cout << "Logic Server listening on " << server_address << std::endl;
    db_stub = DBService::NewStub(grpc::CreateChannel("localhost:18900", grpc::InsecureChannelCredentials()));
    while(true){
        // HandleRpcs();
        tick();
        sleep(3);
    }
}