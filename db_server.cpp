#include <iostream>
#include <memory>
#include <string>
#include <grpc/support/log.h>
#include <grpcpp/grpcpp.h>
#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/json.hpp>

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
    new InsertOneCallData(&service_, cq_.get(), &client_);
    new FindOneCallData(&service_, cq_.get(), &client_);
    void* tag;
    bool ok;
    while (true) {
        // std::cout << tag << " 0 " << ok << std::endl;
        GPR_ASSERT(cq_->Next(&tag, &ok));
        // std::cout << tag << " 1 " << ok << std::endl;
        GPR_ASSERT(ok);
        // std::cout << tag << " 2 " << ok << std::endl;
        static_cast<CallData*>(tag)->Proceed();
        // std::cout << tag << " 3 " << ok << std::endl;
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

void db_server::ServerImpl::InsertOneCallData::doProceed() {
    if (status_ == CREATE) {
        status_ = PROCESS;
        service_->Requestinsert_one(&ctx_, &request_, &responder_, cq_, cq_, this);
    } else if (status_ == PROCESS) {
        new InsertOneCallData(service_, cq_, client_);
        mongocxx::database db = client_->database(request_.db_name());
        mongocxx::collection coll = db[request_.col_name()];
        std::cout << request_.doc() << std::endl;
        bsoncxx::document::value doc_value = bsoncxx::from_json(request_.doc());
        bsoncxx::stdx::optional<mongocxx::result::insert_one> result = coll.insert_one(bsoncxx::document::view_or_value(doc_value));
        std::cout << "insert result: " << result << std::endl;
        std::cout << "InsertOneCallData" << std::endl;
        reply_.set_message("insert success");
        reply_.set_status(request_.doc());
        status_ = FINISH;
        responder_.Finish(reply_, Status::OK, this);
    } else {
        GPR_ASSERT(status_ == FINISH);
        delete this;
    }
}

void db_server::ServerImpl::FindOneCallData::doProceed() {
    if (status_ == CREATE) {
        status_ = PROCESS;
        service_->Requestfind_one(&ctx_, &request_, &responder_, cq_, cq_, this);
        // std::cout << "FindOneCallData Requestfind_one" << std::endl;
    } else if (status_ == PROCESS) {
        new FindOneCallData(service_, cq_, client_);
        status_ = FINISH;
        std::cout << "FindOneCallData" << std::endl;
        reply_.set_message("find_one success");
        reply_.set_status(request_.query());
        responder_.Finish(reply_, Status::OK, this);
    } else {
        GPR_ASSERT(status_ == FINISH);
        delete this;
    }
}
