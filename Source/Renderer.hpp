#ifndef PS2E_RENDERER_HPP
#define PS2E_RENDERER_HPP

#include <draw_buffers.h>
#include <tamtypes.h>

class DrawState{
public:
    int width;
    int height;
    int vmode;
    int gmode;
    framebuffer_t frameBuffer;
    zbuffer_t zBuffer;

    static qword_t* gCmdBuffer;
    static constexpr inline int gCmdBufferSize = 40 * sizeof(qword_t);
};

namespace Renderer{
    int gs_init(DrawState& ds, int psm, int psmz);

    static constexpr inline int gOffsetX = 2048;
    static constexpr inline int gOffsetY = 2048;
}

#endif //!PS2E_RENDERER_HPP
