#ifndef __AX_RAW_OSTREAM_HPP__
#define __AX_RAW_OSTREAM_HPP__

#include <functional>
#include <vector>
#include <string>
#include <fstream>
#include <tuple>
#include "ax_binary_hbo_serialize.hpp"
#include "ax_binary_nbo_serialize.hpp"
#include "ax_buffer_writter.hpp"
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
 *  usage:
 *       auto stream = hbo::binary_stream (vector|string|ostream)
 *       stream << value
 *       stream.put (value)
 *   this stream does not own the object it wraps 
 *   so the object must stay alive until you're 
 *   done with the stream
 */
  
namespace ax{
    namespace util {

        template <template <typename> class _Serialize = hbo::serialize>
        struct basic_binary_ostream {           
            template <typename _Writter>
            basic_binary_ostream (_Writter &&writter_)
            :   writter_ (make_writter (
                    std::forward<_Writter> (writter_))),
                bad_bit_ (false)
            {}

            inline basic_binary_ostream &write (void const *data_, std::size_t size_) {
                auto written_ = reinterpret_cast<buffer_writter_base &>(writter_) (data_, size_);
                bad_bit_ = (written_ != size_);
                return *this;
            }

            template <typename... _Vargs>
            inline basic_binary_ostream &operator () (_Vargs&&...args_) {
                return write (std::forward<_Vargs> (args_)...);
            }

            template <typename _Vtype, typename _Utype = 
                typename std::remove_reference<_Vtype>::type>
            inline basic_binary_ostream &put (_Vtype &&val_) {
                return _Serialize<_Utype>::write (*this, val_);
            }

            template <typename _Vtype>
            inline basic_binary_ostream &operator << (_Vtype &&value_) {
                return put (std::forward<_Vtype> (value_));
            }

            inline operator bool () const {
                return !fail ();
            }

            inline bool fail () const {
                return bad_bit_;
            }

            inline basic_binary_ostream &clear () {
                bad_bit_ = false;
                return *this;
            }

        private:           
            typedef std::pair<void *, void *> _Vptr;

            template <typename _Vtype, typename _Utype = 
                typename std::remove_reference<_Vtype>::type>
            static inline auto make_writter (_Vtype &&writter_) {
                auto vptr_ = _Vptr {nullptr, nullptr};
                new (&vptr_) buffer_writter<_Utype> (
                    std::forward<_Vtype> (writter_));
                return vptr_;
            }

        private:
            _Vptr writter_;
            bool bad_bit_;            
        };

        namespace hbo {
            typedef basic_binary_ostream<hbo::serialize> binary_ostream;
        }

        namespace nbo {
            typedef basic_binary_ostream<nbo::serialize> binary_ostream;
        }

        using hbo::binary_ostream;

    }
}

#endif
