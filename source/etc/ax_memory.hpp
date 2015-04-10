#ifndef __AX_UTILS_MEMORY_HPP__
#define __AX_UTILS_MEMORY_HPP__

#include <memory>

namespace ax {
    namespace util {
        template <typename Type, typename U = typename std::remove_reference<Type>::type>
        auto move_unique (Type &&in) {
            return std::make_unique<U> (std::forward<Type> (in));
        }

        template <typename Type, typename U = typename std::remove_reference<Type>::type>
        auto move_shared (Type &&in) {
            return std::make_shared<U> (std::forward<Type> (in));
        }

    }
}

#endif

