/* This file is part of mediaserver. A webrtc sfu server.
 * Copyright (C) 2018 Arvind Umrao <akumrao@yahoo.com> & Herman Umrao<hermanumrao@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */



#include "net/TcpServer.h"
#include "base/logger.h"
#include "base/application.h"
#include "net/PortManager.h"
#include <inttypes.h>
#include "net/IP.h"
#include "net/SslConnection.h"


#define IPC_PIPE_NAME "pearl_server_ipc"
#define BACKLOG 128
#define THREAD_COUNT 4

namespace base
{
    namespace net
    {
        
        struct UserInfo {
            int id;
            pthread_t tid;
            TcpServerBase *obj;
        };


        /* Static methods for UV callbacks. */

        inline static void onConnection(uv_stream_t* handle, int status) {
            auto* server = static_cast<TcpServerBase*> (handle->data);

            if (server == nullptr)
                return;

            server->OnUvConnection(handle , status);
        }

        inline static void onClose(uv_handle_t* handle) {
            SInfo << " TcpServerBase::onClose"  ;
            delete handle;
        }
        
   
      
       
       static void __on_http_connection(uv_stream_t *listener, const int status) {

            UserInfo* usrInfo = (UserInfo*) listener->data;
            int e;

            if (0 != status)
            {
                SError << status;
            }
            
          //  auto* server = static_cast<TcpServerBase*> (listener->data);
             
            SInfo << "__on_http_connection  thread id " << usrInfo->id << " tid " << usrInfo->tid  << " loop " << listener->loop;
            
            usrInfo->obj->OnUvConnection(listener , status);
            
            return;

            //    uv_tcp_t *conn = ( uv_tcp_t *) calloc(1, sizeof(*conn));
            //
            //    e = uv_tcp_init(listener->loop, conn);
            //    if (0 != status)
            //        uv_fatal(e);
            //
            //    e = uv_accept(listener, (uv_stream_t*)conn);
            //    if (0 != e)
            //        uv_fatal(e);
            //    

            

            uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof (uv_tcp_t));
            client->data = listener->data;
            uv_tcp_init(listener->loop, client);
            if (uv_accept(listener, (uv_stream_t*) client) == 0) {
                SInfo << __func__;
//                uv_read_start((uv_stream_t*) client, alloc_buffer, echo_read);
            } else {
 //               uv_close((uv_handle_t*) client, on_close);
            }


            //  h2o_socket_t *sock =
            //    h2o_uv_socket_create((uv_stream_t*)conn, (uv_close_cb)free);

            // struct timeval connected_at = *h2o_get_timestamp(&thread->ctx, NULL, NULL);

            //  thread->accept_ctx.ctx = &thread->ctx;
            //  thread->accept_ctx.hosts = sv->cfg.hosts;
            //  h2o_http1_accept(&thread->accept_ctx, sock, connected_at);
        }

        static void __worker_start(void* _worker) {
            // assert(uv_tcp);

            uv_multiplex_worker_t* worker = (uv_multiplex_worker_t*) _worker;

            uv_tcp_t* listener = (uv_tcp_t*) & worker->listener;
            UserInfo* usrInfo = (UserInfo*) listener->data;

            usrInfo->tid = worker->thread;

            //  h2o_context_init(&thread->ctx, listener->loop, &sv->cfg);

            int e = uv_listen((uv_stream_t*) listener, BACKLOG,
                    __on_http_connection);
            if (e != 0)
                SError << e;

            while (1)
                uv_run(listener->loop, UV_RUN_DEFAULT);
        }

        
        
        /* Instance methods. */

        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)

        TcpServerBase::TcpServerBase(uv_tcp_t* uvHandle, int backlog, bool multiThreaded) : uvHandle(uvHandle), multithreaded(multiThreaded) {

            int err;
            int i;

           // 
           // setup_workers();
            
            if(multithreaded)
            {
                    uv_multiplex_init(&m, uvHandle, IPC_PIPE_NAME, THREAD_COUNT, __worker_start);

                   // UserInfo *threads = calloc(sv->nworkers + 1, sizeof(UserInfo));
                   // if (!sv->threads)
                   //   exit(-1);

                   /* If there are not enough resources to sustain our workers, we abort */
                   for (i = 0; i < THREAD_COUNT; i++) {

                       UserInfo *userInf = new UserInfo();
                       userInf->id = i + 1;
                       userInf->obj = this;
                       uv_multiplex_worker_create(&m, i, userInf);
                   }
                   if(  m.listener->loop != Application::Application::uvGetLoop())
                   {
                      SError<< " wrong loop" ;
                   }
                   uv_multiplex_dispatch(&m);
            }
            else
            {
            
 
                this->uvHandle->data = (void*) this;

                err = uv_listen(
                        reinterpret_cast<uv_stream_t*> (this->uvHandle),
                        backlog,
                        static_cast<uv_connection_cb> (onConnection));

                if (err != 0)
                {
                    uv_close(reinterpret_cast<uv_handle_t*> (this->uvHandle), static_cast<uv_close_cb> (onClose));

                    LError("uv_listen() failed: %s", uv_strerror(err));
                }

                // Set local address.
                if (!SetLocalAddress())
                {
                    uv_close(reinterpret_cast<uv_handle_t*> (this->uvHandle), static_cast<uv_close_cb> (onClose));

                    LError("error setting local IP and port");
                }
            }
        }

        TcpServerBase::~TcpServerBase() {


            if (!this->closed)
                Close();
        }

        void TcpServerBase::Close() {


            if (this->closed)
                return;

            this->closed = true;

            // Tell the UV handle that the TcpServerBase has been closed.
            this->uvHandle->data = nullptr;

            SInfo << " TcpServerBase::Close,  closing all active connections"  <<  this->connections.size();

            for (auto* connection : this->connections)
            {
                delete connection;
            }

            uv_close(reinterpret_cast<uv_handle_t*> (this->uvHandle), static_cast<uv_close_cb> (onClose));
        }

        void TcpServerBase::Dump() const {
            LDebug("<TcpServerBase>");
            LDebug(
                    "  [TCP, local:%s :%" PRIu16 ", status:%s, connections:%zu]",
                    this->localIp.c_str(),
                    static_cast<uint16_t> (this->localPort),
                    (!this->closed) ? "open" : "closed",
                    this->connections.size());
            LDebug("</TcpServerBase>");
        }

        bool TcpServerBase::SetLocalAddress() {


            int err;
            int len = sizeof (this->localAddr);

            err =
                    uv_tcp_getsockname(this->uvHandle, reinterpret_cast<struct sockaddr*> (&this->localAddr), &len);

            if (err != 0)
            {
                LError("uv_tcp_getsockname() failed: %s", uv_strerror(err));

                return false;
            }

            int family;

            IP::GetAddressInfo(
                    reinterpret_cast<struct sockaddr*> (&this->localAddr), family, this->localIp, this->localPort);

            return true;
        }

        inline void TcpServerBase::OnUvConnection(uv_stream_t* listener, int status) {


            if (this->closed)
                return;

            int err;

            if (status != 0)
            {
                LError("error while receiving a new TCP connection: %s", uv_strerror(status));

                return;
            }
            
            


        // Notify the subclass so it provides an allocated derived class of TCPConnection.
            TcpConnectionBase* connection = nullptr;
            UserOnTcpConnectionAlloc(&connection);

            ASSERT(connection != nullptr);

            try
            {
                connection->Setup(this, listener->loop, &(this->localAddr), this->localIp, this->localPort);
            } catch (const std::exception& error)
            {
                delete connection;

                return;
            }

            // Accept the connection.
            err = uv_accept(
                   listener,
                    reinterpret_cast<uv_stream_t*> (connection->GetUvHandle()));

            if (err != 0)
                LError("uv_accept() failed: %s", uv_strerror(err));

            // Start receiving data.
            try
            {
                // NOTE: This may throw.
                connection->Start();
            } catch (const std::exception& error)
            {
                delete connection;

                return;
            }

            // Notify the subclass and delete the connection if not accepted by the subclass.
            if (UserOnNewTcpConnection(connection))
            {
                   SInfo << "TcpServerBase new connection "  << connection;
                this->connections.insert(connection);
            }
            else
                delete connection;

      
            
            
        }
        


        void TcpServerBase::OnTcpConnectionClosed(TcpConnectionBase* connection) {

            
            SInfo << " TcpConnectionBase close "  << connection;
              

            // Remove the TcpConnectionBase from the set.
            this->connections.erase(connection);

            // Notify the subclass.
             UserOnTcpConnectionClosed(connection);

            // Delete it.
            delete connection;
        }
        
        uv_tcp_t* TcpServerBase::BindTcp(std::string &ip, int port) {
            //Arvind
            //please do not do it here . Drive your own class from TServerBase. 
            //Use Portmanager there itself. See the sfu example
            
//	    if(port == -1)
//            {
//                return PortManager::BindTcp(ip);
//            }
            int bind_flags = 0;
            uv_tcp_t *uvHandle = new uv_tcp_t;
            struct sockaddr_in6 addr6;
            struct sockaddr_in addr;
         
            int r;

            r = uv_tcp_init(Application::uvGetLoop(), uvHandle);
            ASSERT(r == 0);

            if (IP::GetFamily(ip) == AF_INET6)
            {
                bind_flags = UV_TCP_IPV6ONLY;
                ASSERT(0 == uv_ip6_addr(ip.c_str(), port, &addr6));
                r = uv_tcp_bind(uvHandle, (const struct sockaddr*) &addr6, bind_flags);
                ASSERT(r == 0);
            } else
            {
                ASSERT(0 == uv_ip4_addr(ip.c_str(), port, &addr));
                r = uv_tcp_bind(uvHandle, (const struct sockaddr*) &addr, bind_flags);
                ASSERT(r == 0);

            }
            
            LTrace("Binded to port ", ip , ":", port);

            return uvHandle;
        }

        /******************************************************************************************************************/
        static constexpr size_t MaxTcpConnectionsPerServer{ 1000};

        /* Instance methods. */

        TcpServer::TcpServer(Listener* listener, std::string ip, int port,  bool multiThreaded, bool ssl )
        : TcpServerBase(BindTcp(ip, port), 256, multiThreaded), listener(listener),ssl(ssl){

        }

        TcpServer::~TcpServer() {

            if (uvHandle)
                delete uvHandle;
            //UnbindTcp(this->localIp, this->localPort); // please do not do it here . Drive your own class from TServerBase.
        }

  
        void TcpServer::UserOnTcpConnectionAlloc(TcpConnectionBase** connection) {

// condition
            // Allocate a new RTC::TcpConnection for the TcpServer to handle it.
            if(ssl)
             *connection = new SslConnection( true);
            else
            *connection = new TcpConnectionBase(listener);
            
            //SInfo << "TcpServer::UserOnTcpConnectionAlloc new connection "  << *connection;
        }

        bool TcpServer::UserOnNewTcpConnection(TcpConnectionBase* connection) {


            if (GetNumConnections() >= MaxTcpConnectionsPerServer)
            {
                LError("cannot handle more than %zu connections", MaxTcpConnectionsPerServer);

                return false;
            }

            return true;
        }

        void TcpServer::UserOnTcpConnectionClosed(TcpConnectionBase* connection) {

            //override this function
        }

    } // namespace net
} // namespace base
