#ifndef __AX_BUFFER_WRITTER_HPP__
#define __AX_BUFFER_WRITTER_HPP__

#include <cstddef>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace ax {
    namespace util{            
            struct buffer_writter_base {
                virtual std::size_t operator () (void const *data_, std::size_t size_) = 0;
                virtual ~buffer_writter_base () = default;                
            };

            template <typename _Vtype>
            struct buffer_writter: 
                buffer_writter_base
            {
                typedef typename std::remove_reference<_Vtype>::type _Writter;

                buffer_writter (_Writter &writter_):
                    writter_ (writter_)
                {}

                std::size_t operator () (void const *data_, std::size_t size_) override {
                    return writter_ (data_, size_);
                }

            private:
                _Writter &writter_;
            };


            template <typename _Ctype>
            struct buffer_writter<std::basic_string<_Ctype>>:
                buffer_writter_base
            {
                typedef std::basic_string<_Ctype> _Container;
                buffer_writter (_Container &container_):
                    container_ (container_)
                {}

                std::size_t operator () (void const *pdata_, std::size_t size_) override {
                    auto wlen_ = size_/sizeof (_Ctype);
                    auto rlen_ = size_%sizeof (_Ctype);
                    auto data_ = reinterpret_cast<_Ctype const *>(pdata_);
                    container_.append (data_, wlen_);
                    if (rlen_ > 0) {
                        auto temp_ = _Ctype ();
                        std::memcpy (&temp_, &data_ [wlen_], rlen_);
                        container_.append (&temp_, 1);
                    }
                    return size_;
                }
            private:
                _Container &container_;
            };
            
            template <typename _Vtype>
            struct buffer_writter<std::vector<_Vtype>>:
                buffer_writter_base
            {
                typedef std::vector<_Vtype> _Container;
                buffer_writter (_Container &container_):
                    container_ (container_)
                {}

                std::size_t operator () (void const *pdata_, std::size_t size_) override {
                    auto wlen_ = size_/sizeof (_Vtype);
                    auto rlen_ = size_%sizeof (_Vtype);
                    auto data_ = reinterpret_cast<_Vtype const *>(pdata_);                            
                    container_.insert (vector_.end (), data_, data_+wlen_);
                    if (rlen_ > 0) {
                        auto temp_ = _Vtype ();
                        std::memcpy (&temp_, &data_ [wlen_], rlen_);
                        container_.emplace_back (temp_);
                    }
                    return size_;
                }
            private:
                _Container &container_;
            };

            template <typename _Vtype>
            struct buffer_writter<std::basic_ostream<_Vtype>>:
                buffer_writter_base
            {
                typedef std::basic_ostream<_Vtype> _Container;
                buffer_writter (_Container &container_):
                    container_ (container_)
                {}

                std::size_t operator () (void const *pdata_, std::size_t size_) override {
                    auto wlen_ = size_/sizeof (_Vtype);
                    auto rlen_ = size_%sizeof (_Vtype);
                    auto data_ = reinterpret_cast<_Vtype const *>(pdata_);                            
                    container_.write (data_, wlen_);
                    if (rlen_ > 0) {
                        auto temp_ = _Vtype ();                                
                        container_.write (&temp_, 1);
                    }
                    return size_;
                }
            private:
                _Container &container_;
            };

            template <typename _Vtype>
            struct buffer_writter<std::basic_ofstream<_Vtype>>:
                buffer_writter<std::basic_ostream<_Vtype>>
            {                
                buffer_writter (std::basic_ofstream<_Vtype> &container_)
                :   buffer_writter<std::basic_ostream<_Vtype>> (container_)
                {}
            };

            template <typename _Vtype>
            struct buffer_writter<std::basic_stringstream<_Vtype>>:
                buffer_writter<std::basic_ostream<_Vtype>>
            {                
                buffer_writter (std::basic_stringstream<_Vtype> &container_)
                :   buffer_writter<std::basic_ostream<_Vtype>> (container_)
                {}
            };

    }
}


#endif
