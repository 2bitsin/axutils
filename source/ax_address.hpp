#ifndef __AX_ADDRESS_HPP__
#define __AX_ADDRESS_HPP__

#include <cstdint>
#include <string>

namespace ax {

    struct address {
        static const std::size_t BUFFER_SIZE = 28;
        static address parse (std::string const &addr_);
        address ();
        address (std::uint16_t port_, std::string const &addr_ = "0.0.0.0");
        address (void const *data_, std::size_t length_);
        ~address ();		
        template <typename Type> Type const *get () const;		
        std::size_t length () const;
        int family () const;
        std::string host () const;
        std::uint16_t port () const;
        std::string string () const;

    private:		
        template <typename Type> Type *get ();
        std::uint8_t buffer_ [BUFFER_SIZE];
        std::size_t length_;
    };

    template <typename Type>
        inline Type const *address::get () const {
            return reinterpret_cast<Type const *>(buffer_);
        }

    template <typename Type>
        inline Type *address::get () {
            return reinterpret_cast<Type *>(buffer_);
        }

}

#endif