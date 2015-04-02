#ifndef __AX_RAW_OSTREAM_HPP__
#define __AX_RAW_OSTREAM_HPP__

#include <functional>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
/*  
 *  raw_ostream is a wrapper on top of other 
 *  stl containers, file streams or custom callbacks
 *  to turn it into a raw binary stream with an
 *  interface similar to that of std::ostream.
 *  This is meant to help serialize basic types 
 *  into binary data form, rather then text form.
 *  If using a container or file with a specific 
 *  char/value_type data will be padded to align 
 *  to size of that particular type.
 */
  
namespace ax{
    namespace util {
        
        struct raw_ostream {
            enum {
                network_byte_order // Makes the stream swap bytes into network byte order for little endian machines
            };

            template <typename _Writter, typename _UWritter = 
                typename std::remove_reference<_Writter>::type>
                raw_ostream (_Writter &&writter_, std::uint32_t flags_ = 0u)
                :   writter_ (wrapper<_UWritter>::get (
                        std::forward<_Writter>(writter_))),
                    flags_ (flags_),
                    bad_bit_ (false)
                {}

            inline raw_ostream &write (void const *data_, std::size_t size_) {
                auto written_ = writter_ (data_, size_);
                bad_bit_ = (written_ != size_);
                return *this;
            }

            inline operator bool () const {
                return !fail ();
            }

            inline bool fail () const {
                return bad_bit_;
            }

            inline raw_ostream &clear () {
                bad_bit_ = false;
                return *this;
            }

        private:           
            typedef std::size_t writter_format_t (void const *data_, std::size_t size_);

            template <typename _Vtype>
                struct wrapper {
                    template <typename _Utype>
                    static std::function<writter_format_t> get (_Utype &&call_){
                        return std::forward<_Utype> (call_);
                    }
                };

            template <typename _Ctype>
                struct wrapper<std::basic_string<_Ctype>> {
                    template <typename _Utype>
                    static std::function<writter_format_t> get (_Utype &&string_){
                        return [&string_] (void const *pdata_, std::size_t size_) {
                            auto wlen_ = size_/sizeof (_Ctype);
                            auto rlen_ = size_%sizeof (_Ctype);
                            auto data_ = reinterpret_cast<_Ctype const *>(pdata_);
                            string_.append (data_, wlen_);
                            if (rlen_ > 0) {
                                auto temp_ = _Ctype ();
                                std::memcpy (&temp_, &data_ [wlen_], rlen_);
                                string_.append (&temp_, 1);
                            }
                            return size_;
                        };
                    }
                };

            template <typename _Vtype>
                struct wrapper<std::vector<_Vtype>> {
                    template <typename _Utype>
                    static std::function<writter_format_t> get (_Utype &&vector_){
                        return [&vector_] (void const *pdata_, std::size_t size_) {
                            auto wlen_ = size_/sizeof (_Vtype);
                            auto rlen_ = size_%sizeof (_Vtype);
                            auto data_ = reinterpret_cast<_Vtype const *>(pdata_);                            
                            vector_.insert (vector_.end (), data_, data_+wlen_);
                            if (rlen_ > 0) {
                                auto temp_ = _Vtype ();
                                std::memcpy (&temp_, &data_ [wlen_], rlen_);
                                vector_.emplace_back (temp_);
                            }
                            return size_;
                        };
                    }
                };

            template <typename _Vtype>
                struct wrapper<std::basic_ofstream<_Vtype>> {
                    template <typename _Utype>
                    static std::function<writter_format_t> get (_Utype &&file_){
                        return [&file_] (void const *pdata_, std::size_t size_) {
                            auto wlen_ = size_/sizeof (_Vtype);
                            auto rlen_ = size_%sizeof (_Vtype);
                            auto data_ = reinterpret_cast<_Vtype const *>(pdata_);                            
                            file_.write (data_, wlen_);
                            if (rlen_ > 0) {
                                auto temp_ = _Vtype ();                                
                                file_.write (&temp_, 1);
                            }
                            return size_;
                        };
                    }
                };

            std::function<writter_format_t> writter_;
            std::uint32_t flags_;
            bool bad_bit_;
        };
    }
}

#endif
