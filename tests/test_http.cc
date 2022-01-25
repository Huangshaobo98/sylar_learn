#include "../src/http/http.h"
#include "../src/log.h"

void test_request() {
    sylar::http::HttpRequest::ptr req(new sylar::http::HttpRequest);
    req->setHeader("host", "www.sylar.top");
    req->setBody("hello sylar");

    req->dump(std::cout) << std::endl;
}

void test_response() {
    sylar::http::HttpResponse::ptr rsp(new sylar::http::HttpResponse);
    rsp->setHeader("aaa", "bbb");
    rsp->setBody("hello sylar");

    rsp->dump(std::cout) << std::endl;
}

int main() {
    test_response();
    return 0;
}