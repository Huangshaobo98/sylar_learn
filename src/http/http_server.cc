#include "http_server.h"
#include "../log.h"
namespace sylar {
namespace http {
    static sylar::Logger::ptr g_logger = __LOG_NAME("system");
    HttpServer::HttpServer(bool keepalive
                , sylar::IOManager* worker, sylar::IOManager* accept_worker)
        :TcpServer(worker, accept_worker)
        ,m_isKeepalive(keepalive) {
    }
    void HttpServer::handleClient(Socket::ptr client) {
        HttpSession::ptr session(new HttpSession(client));
        do {
            auto req = session->recvRequest();
            if(!req) {
                __LOG_WARN(g_logger) << "recv http request fail, errno="
                    << errno << " strerr=" << strerror(errno) << " client"
                    << *client;
                    break;
            }
            HttpResponse::ptr rsp(new HttpResponse(req->getVersion(), req->isClose() || !m_isKeepalive));
            rsp->setBody("hello sylar");
            __LOG_INFO(g_logger) << "request:" << std::endl
                << *req;
            __LOG_INFO(g_logger) << "response:" << std::endl
                << *rsp;
            session->sendReponse(rsp);
        } while(m_isKeepalive);
    }
}
}