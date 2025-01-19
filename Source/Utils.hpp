#ifndef PS2E_UTILS_HPP
#define PS2E_UTILS_HPP

#include <cstdint>
#include <stddef.h>
#include <stdio.h>

#include <tamtypes.h>

namespace Utils{
    void PrintBuffer(qword_t* buffer, size_t len){
        printf("-- buffer\n");
        for(size_t i = 0; i < len; i++){
            printf("%016llx %016llx\n", buffer->dw[0], buffer->dw[1]);
            buffer++;
        }
        printf("-- /buffer\n");
    }

    constexpr inline int64_t ShiftAsI64(int value, int shiftAmount){ return static_cast<int64_t>(value) << shiftAmount; };
}

#endif //!PS2E_UTILS_HPP