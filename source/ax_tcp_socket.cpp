#include "ax_address.hpp"
#include "ax_tcp_socket.hpp"

#include <WinSock2.h>
#include <WS2tcpip.h>
#undef ERROR

using namespace ax;

#include <WinSock2.h>
#include <Windows.h>
#include <stdexcept>

#pragma comment(lib, "ws2_32.lib")

struct __ws2_32_init {
    __ws2_32_init () {
        WSADATA wsa;
        if (WSAStartup (MAKEWORD(2,2), &wsa)) {
            MessageBox (0, "Couldn't load WS2_32.DLL", "Error", MB_ICONERROR);
            std::terminate ();
        }
    }
    ~__ws2_32_init () {
        WSACleanup ();
    }
};


static std::intptr_t __new_tcp_socket (int af = AF_INET) {	
    static __ws2_32_init __init = __ws2_32_init ();
    auto desc_ = ::socket (af, SOCK_STREAM, IPPROTO_TCP);
    if (desc_ == tcp::socket::NULL_SOCKET)
        throw std::runtime_error ("Error while creating socket");
    return desc_;
}

tcp::socket tcp::socket::connect (ax::address const &address_) {
    socket sock_ (__new_tcp_socket (address_.family ()), address_);
    if (!::connect (sock_.raw_socket (), address_.get<sockaddr> (), int (address_.length ())))
        return sock_;
    throw std::runtime_error ("Error while connecting to " + address_.host ());
}

tcp::socket tcp::socket::listen (ax::address const &address_, int backlog_) {
    socket sock_ (__new_tcp_socket (address_.family ()), address_);
    if (::bind (sock_.raw_socket (), address_.get<sockaddr> (), int (address_.length ())))
        throw std::runtime_error ("Error while binding socket to " + address_.host ());
    if (::listen (sock_.raw_socket (), backlog_)) 
        throw std::runtime_error ("Error while trying to listen on " + address_.host ());
    return sock_;
}

ax::tcp::socket ax::tcp::socket::accept () const {
    union {
        std::uint8_t buff_ [32];
        sockaddr adr_;
    };	
    auto len_ = int (sizeof (buff_));
    auto des_ = ::accept (raw_socket (), &adr_, &len_);	
    if (des_ != NULL_SOCKET)
        return ax::tcp::socket (des_, ax::address (&adr_, len_));	
    throw std::runtime_error ("Error while accepting connection");
}

tcp::socket::socket (
    std::intptr_t desc_, 
    ax::address const &addr_)
:	desc_ (desc_),
    addr_ (addr_),
    nothrow_ (false)
{}

ax::tcp::socket::socket ()
: 	desc_ (NULL_SOCKET),
    addr_ (),
    nothrow_ (false)
{}

tcp::socket::socket (socket &&sock_):
    socket ()
{
    std::swap (desc_, sock_.desc_);
    std::swap (addr_, sock_.addr_);
    std::swap (nothrow_, sock_.nothrow_);
}

tcp::socket &tcp::socket::operator = (socket &&sock_) {
    *this && close ();		
    std::swap (desc_, sock_.desc_);
    std::swap (addr_, sock_.addr_);
    std::swap (nothrow_, sock_.nothrow_);
    return *this;
}

tcp::socket::~socket () {
    *this && close ();
}

std::intptr_t ax::tcp::socket::detatch () {
    auto d = desc_;
    desc_ = NULL_SOCKET;
    addr_ = address ();
    nothrow_ = false;
    return d;
}

std::intptr_t ax::tcp::socket::raw_socket () const {
    return desc_;
}

std::size_t ax::tcp::socket::pending () const {
    u_long arg;
    if (::ioctlsocket (raw_socket (), FIONREAD, &arg)) {
        throw std::runtime_error ("pending(): ioctl failure");
    }
    return arg;
}

ax::tcp::socket &ax::tcp::socket::blocking (bool blocking_) {
    u_long arg = !blocking_;
    if (::ioctlsocket (raw_socket (), FIONBIO, &arg)) {
        throw std::runtime_error ("blocking(): ioctl failure");
    }
    return *this;
}

ax::address const &ax::tcp::socket::address () const {
    return addr_;
}

ax::tcp::socket &
    ax::tcp::socket::shutdown (int what_) 
{	
    what_ = (what_ & SEND ? SD_SEND : 0)|
        (what_ & RECV ? SD_RECEIVE : 0);
    if (!::shutdown (raw_socket (), what_))
        return *this;
    throw std::runtime_error ("Can't shutdown socket");
}

ax::tcp::socket &
    ax::tcp::socket::close () 
{	
    if (!::closesocket (raw_socket ())) 
        return *this;
    throw std::runtime_error ("Can't close socket");
}

std::intptr_t ax::tcp::socket::recv (void * buffer_, std::intptr_t len_, bool oob_) {
    return __is_closed (::recv (raw_socket (), reinterpret_cast<char *>(buffer_), int (len_), oob_ ? MSG_OOB : 0));
}

std::intptr_t ax::tcp::socket::peek (void * buffer_, std::intptr_t len_, bool oob_) {
    return __is_closed (::recv (raw_socket (), reinterpret_cast<char *>(buffer_), int (len_), MSG_PEEK | (oob_ ? MSG_OOB : 0)));
}

std::intptr_t ax::tcp::socket::send (void const * buffer_, std::intptr_t len_, bool oob_) {
    return __is_closed (::send (raw_socket (), reinterpret_cast<char const *>(buffer_), int (len_), oob_ ? MSG_OOB : 0));
}

ax::tcp::socket::operator bool () const {	
    return desc_ != NULL_SOCKET;
}

ax::tcp::socket &ax::tcp::socket::__set_option (int level, int option, void const * data, int length) {
    if (!::setsockopt (raw_socket (), level, option, reinterpret_cast<char const *>(data), length))
        return *this; 
    throw std::runtime_error ("setsockopt failure");
}

ax::tcp::socket &ax::tcp::socket::__get_option (int level, int option, void * data, int *length) {
    if (!::getsockopt (raw_socket (), level, option, reinterpret_cast<char *>(data), length))
        return *this; 
    throw std::runtime_error ("getsockopt failure");
}

std::intptr_t ax::tcp::socket::__is_closed (std::intptr_t in_) {	
    switch (in_) {
    case CLOSED:
        desc_ = NULL_SOCKET;
        if (!nothrow_) throw std::runtime_error (
            "Socked closed");
        return in_;
    case ERROR:
        if (!nothrow_) throw std::runtime_error (
            "Socked error");			
        return in_;
    }
    return in_;
}

ax::tcp::socket &ax::tcp::socket::timeout (std::uint32_t ms, int what_) {
    if (what_ & ax::tcp::socket::SEND) 
        __set_option (SOL_SOCKET, SO_SNDTIMEO, &ms, sizeof (ms));
    if (what_ & ax::tcp::socket::RECV)
        __set_option (SOL_SOCKET, SO_RCVTIMEO, &ms, sizeof (ms));
    return *this;
}

ax::tcp::socket &ax::tcp::socket::keepalive (bool ka) {
    return __set_option (SOL_SOCKET, SO_KEEPALIVE, &ka, sizeof (ka));
}

ax::tcp::socket &ax::tcp::socket::dontthrow (bool _) {
    nothrow_ = _;
    return *this;
}


int const ax::tcp::socket::LISTEN_MAX_BACKLOG = SOMAXCONN;
std::intptr_t const ax::tcp::socket::NULL_SOCKET = INVALID_SOCKET;
