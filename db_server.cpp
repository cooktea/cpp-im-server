#include <iostream>
#include <memory>
#include <string>

#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>
#include "cpp_im_server.grpc.pb.h"
#include "db_server.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;
using cpp_im_server::DBService;
using cpp_im_server::InsertRequest;
using cpp_im_server::InsertReply;

void db_server::ServerImpl::HandleRpcs() {
    new ServerImpl::CallData(&service_, cq_.get());
    void* tag;
    bool ok;
    while (true) {
        GPR_ASSERT(cq_->Next(&tag, &ok));
        GPR_ASSERT(ok);
        static_cast<CallData*>(tag)->Proceed();
    }
}

void db_server::ServerImpl::Run(int port) {
    std::string server_address = std::string("0.0.0.0:") + std::to_string(port);
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service_);
    cq_ = builder.AddCompletionQueue();
    server_ = builder.BuildAndStart();
    std::cout << "DB Server listening on " << server_address << std::endl;
    HandleRpcs();
}

void db_server::ServerImpl::CallData::Proceed() {
    if (status_ == CREATE) {
        status_ = PROCESS;
        service_->Requestinsert_one(&ctx_, &request_, &responder_, cq_, cq_, this);
    } else if (status_ == PROCESS) {
        new CallData(service_, cq_);
        status_ = FINISH;
        reply_.set_message("insert success");
        reply_.set_status(request_.doc());
        responder_.Finish(reply_, Status::OK, this);
    } else {
        GPR_ASSERT(status_ == FINISH);
        delete this;
    }
}
