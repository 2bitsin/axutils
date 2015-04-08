#ifndef __AX_SOCKET_STREAM_ADAPTE_HPP__
#define __AX_SOCKET_STREAM_ADAPTE_HPP__

#include <memory>

namespace ax {  
    namespace util {
        template <typename _Ctype>
        struct socket_stream_adapter_base {
            virtual std::intptr_t peek (_Ctype *, std::intptr_t, bool = false) = 0;
            virtual std::intptr_t read (_Ctype *, std::intptr_t, bool = false) = 0;
            virtual std::intptr_t write (_Ctype const *, std::intptr_t, bool = false) = 0;
            virtual std::intptr_t ignore (std::intptr_t, bool = false) = 0;
            virtual ~socket_stream_adapter_base () = default;
        };


        template <typename _Ctype, typename _Socket>
        struct socket_stream_adapter:
            socket_stream_adapter_base<_Ctype>
        {

            socket_stream_adapter (_Socket &socket_):
                socket_ (socket_)
            {}

            std::intptr_t peek (_Ctype *buff_, std::intptr_t size_, bool oob_ = false) override {
                return _adjust (socket_.peek (buff_, size_ * sizeof (_Ctype), oob_));
                
            }

            std::intptr_t read (_Ctype *buff_, std::intptr_t size_, bool oob_ = false) override {
                return _adjust (socket_.recv (buff_, size_ * sizeof (_Ctype), oob_));
                
            }

            std::intptr_t write (_Ctype const *buff_, std::intptr_t size_, bool oob_ = false) override {
                return _adjust (socket_.send (buff_, size_ * sizeof (_Ctype), oob_));                    
            }
            
            std::intptr_t ignore (std::intptr_t size_, bool oob_ = false) override {                    
                auto temp_ = std::make_unique<_Ctype []> (size_);
                return read (temp_.get (), size_, oob_) ;
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
