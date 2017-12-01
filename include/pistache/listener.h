/* listener.h
   Mathieu Stefani, 12 August 2015
   
  A TCP Listener
*/

#pragma once

#include <vector>
#include <memory>
#include <thread>

#include <sys/resource.h>

#include <pistache/tcp.h>
#include <pistache/net.h>
#include <pistache/os.h>
#include <pistache/flags.h>
#include <pistache/async.h>
#include <pistache/reactor.h>

namespace Pistache {
namespace Tcp {

class Peer;
class Transport;

void setSocketOptions(Fd fd, Flags<Options> options);

class Listener {
public:

    struct Load {
        typedef std::chrono::system_clock::time_point TimePoint;
        double global;
        std::vector<double> workers;

        std::vector<rusage> raw;
        TimePoint tick;
    };

    Listener(const size_t & maxSocket = 1000);
    ~Listener();

    Listener(const Address& address, const size_t & maxSocket = 1000);
    void init(
            size_t workers,
            Flags<Options> options = Options::None,
            int backlog = Const::MaxBacklog, 
            const size_t & maxSocket = 1000,
            size_t maxBufferSize = Const::BufferSize);
    void setHandler(const std::shared_ptr<Handler>& handler);

    bool bind();
    bool bind(const Address& adress);

    bool isBound() const;

    void run();
    void runThreaded();

    void shutdown();

    Async::Promise<Load> requestLoad(const Load& old);

    Options options() const;
    Address address() const;

    void pinWorker(size_t worker, const CpuSet& set);

private: 
    Address addr_; 
    int listen_fd;
    int backlog_;
    size_t maxSocket_;
    size_t maxBufferSize_;
    
    NotifyFd shutdownFd;
    Polling::Epoll poller;

    Flags<Options> options_;
    std::unique_ptr<std::thread> acceptThread;

    size_t workers_;
    std::shared_ptr<Transport> transport_;
    std::shared_ptr<Handler> handler_;

    std::shared_ptr<Aio::Reactor> reactor_;
    Aio::Reactor::Key transportKey;

    void handleNewConnection();
    void dispatchPeer(const std::shared_ptr<Peer>& peer);

};

} // namespace Tcp
} // namespace Pistache
