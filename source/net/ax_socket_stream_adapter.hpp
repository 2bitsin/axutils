#ifndef __AX_SOCKET_STREAM_ADAPTE_HPP__
#define __AX_SOCKET_STREAM_ADAPTE_HPP__

#include <memory>
#include <vector>
#include <array>
#include <initializer_list>

namespace ax {  
    namespace util {
        template <typename _Ctype>
        struct socket_stream_adapter_base {
            virtual std::intptr_t peek (_Ctype *, std::intptr_t) = 0;
            virtual std::intptr_t read (_Ctype *, std::intptr_t) = 0;
            virtual std::intptr_t write (_Ctype const *, std::intptr_t) = 0;
            virtual std::intptr_t ignore (std::intptr_t) = 0;
            virtual ~socket_stream_adapter_base () = default;

            bool put (std::basic_string<_Ctype> const &buff_) {
                return write (buff_.c_str (), buff_.length ()) == buff_.length ();
            }

            bool put (std::vector<_Ctype> const &buff_) {
                return write (buff_.data (), buff_.size ()) == buff_.size ();
            }

            template <std::size_t _Count>
            bool put (std::array<_Ctype, _Count> const &buff_) {
                return write (buff_.data (), buff_.size ()) == buff_.size ();
            }

            template <std::size_t _Count>
            bool put (const _Ctype (&buff_) [_Count] ) {
                auto length_ = _Count - (*std::rend(buff_) ? 0 : 1);
                return write (std::begin (buff_), length_) == length_;
            }
            
            bool put (std::initializer_list<_Ctype> const &buff_) {
                return write (buff_.begin (), buff_.size ()) == buff_.size ();
            }
           
            std::shared_ptr<_Ctype> get (std::size_t size_) {
                auto data_ = std::make_shared<_Ctype []> (size_);
                auto gots_ = read (data_.get (), size_);
                if (gots_ == size_)
                    return data_;
                throw std::underflow_error ("Unable to get enough data from socket");
            }

        };


        template <typename _Ctype, typename _Socket, bool _Oob = false>
        struct socket_stream_adapter:
            socket_stream_adapter_base<_Ctype>
        {
            socket_stream_adapter (_Socket &socket_):
                socket_ (socket_)
            {}

            std::intptr_t peek (_Ctype *buff_, std::intptr_t size_) override {
                return _adjust (socket_.peek (buff_, size_ * sizeof (_Ctype), _Oob));
                
            }

            std::intptr_t read (_Ctype *buff_, std::intptr_t size_) override {
                return _adjust (socket_.recv (buff_, size_ * sizeof (_Ctype), _Oob));
                
            }

            std::intptr_t write (_Ctype const *buff_, std::intptr_t size_) override {
                return _adjust (socket_.send (buff_, size_ * sizeof (_Ctype), _Oob));                    
            }
            
            std::intptr_t ignore (std::intptr_t size_) override {                    
                auto temp_ = std::make_unique<_Ctype []> (size_);
                return read (temp_.get (), size_) ;
            };                

        private:

            template <typename _In>
            inline static auto _adjust (_In const &in_) {
                return in_ >= 0 ? in_ / sizeof (_Ctype) : in_;
            }

            _Socket &socket_;                    
        };
            
    }
}

#endif
