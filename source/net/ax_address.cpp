#include "ax_address.hpp"

#include <cstring>
#include <string>
#include <algorithm>
#include <regex>

#include <winsock2.h>
#include <ws2tcpip.h>
#undef min
#undef max

ax::address ax::address::parse (std::string const &addr_and_port_) {
    static const auto ipr_ = std::regex ("^\\[?([^\\[\\]]+)\\]?\\:([0-9]+$)", std::regex::optimize);
    std::smatch sm_;
    if (!std::regex_search (addr_and_port_, sm_, ipr_))
        throw std::runtime_error ("Unable to parse: "+addr_and_port_);
    auto addr_ = sm_ [1].str ();
    auto port_ = sm_ [2].str ();
    return ax::address (std::stoi (port_), addr_);
}

ax::address::address ()
:	length_ (0) 
{
    memset (buffer_, 0, sizeof (buffer_));
}


ax::address::address (
    std::uint16_t port_, 
    std::string const &addr_)
{	
    memset (buffer_, 0, sizeof (buffer_));	
    auto parse_ip_ = [&] (int af_, std::size_t len_) {
        auto *p_ = this->get<sockaddr_in> ();
        if (inet_pton (af_, addr_.c_str (), &p_->sin_addr)) {
            p_->sin_family = af_;
            p_->sin_port = htons (port_);
            length_ = len_;
            return true;
        }
        return false;
    };

    if (!parse_ip_ (AF_INET, sizeof (sockaddr_in)) &&	
        !parse_ip_ (AF_INET6, sizeof (sockaddr_in6)))
    {
        addrinfo *painfo_ = nullptr;
        if (getaddrinfo (addr_.c_str (), nullptr, nullptr, &painfo_))
            throw std::runtime_error ("Wasn't able to resolve address: " + addr_);
        memcpy (buffer_, painfo_->ai_addr, painfo_->ai_addrlen);
        length_ = painfo_->ai_addrlen;
        get<sockaddr_in> ()->sin_port = htons (port_);
        freeaddrinfo (painfo_);
    }	
}

ax::address::address (void const *data_, std::size_t length_):
    length_ (length_)
{
    memcpy (buffer_, data_, std::min (length_, sizeof (buffer_)));
}

ax::address::~address () {}

std::size_t ax::address::length () const {
    return length_;
}

int ax::address::family () const {
    return get<sockaddr> ()->sa_family;
}

std::string ax::address::host () const {
    char host_ [256];
    if (inet_ntop (family (), const_cast<in_addr *>
        (&get<sockaddr_in>()->sin_addr), 
        host_, sizeof (host_)))
    {
        return host_;
    }
    throw std::runtime_error ("Wasn't able to format host.");
}

std::uint16_t ax::address::port () const {
    return ntohs (get<sockaddr_in> ()->sin_port);
}

std::string ax::address::string () const {
    return family () == AF_INET6 ?
        "[" + host () +  "]:" + std::to_string (port ()):
        host () + ":" + std::to_string (port ());
}
