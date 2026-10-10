/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "pe1/psyq_nop.h"
#include "pe1/menu_widget.h"
#include "pe1/menu_queue.h"
#include "pe1/boot_disc_check.h"
#include "pe1/draw_state.h"
#include "pe1/psyq_tim.h"
#include "pe1/draw_buffers.h"

typedef MenuWidgetNode MenuInputWidget;

MenuWidgetNode *MenuWidget_GetCurrentNode(void);

extern int D_8009D0EC;
extern MenuInputQueuedEvent *D_8009D0E0;
extern MenuInputQueuedEvent *D_8009D0E4;
extern MenuInputQueuedEvent * D_8009D0DC;
extern int D_8009D0E8;
extern int D_8009D0F0;
extern int D_8009D0F4;
extern int D_8009D0F8;

extern int g_FieldPadBits[];
#define g_FieldPadBits (g_FieldPadBits[0])

int Draw_RemapStatusFlags(void)
{
  int value;
  register int later_value;
  register int bit2;
  int mask;
  value = g_FieldPadBits;
  mask = (value << 9) & 0x1000;
  if (value & 0x20)
  {
    mask |= 0x4000;
  }
  if (value & 0x40)
  {
    mask |= 0x8000;
  }
  if (value & 0x10)
  {
    mask |= 0x2000;
  }
  if (value & 0x20000000)
  {
    mask |= 0x20;
  }
  if (value & 0x40000000)
  {
    mask |= 0x40;
  }
  if (value & 0x10000000)
  {
    mask |= 0x10;
  }
  if (value < 0)
  {
    mask |= 0x80;
  }
  if (value & 0x04000000)
  {
    mask |= 0x4;
  }
  if (value & 0x08000000)
  {
    mask |= 0x8;
  }
  later_value = value;
  if (later_value & 0x01000000)
  {
    mask |= 0x1;
  }
  if (later_value & 0x02000000)
  {
    mask |= 0x2;
  }
  bit2 = later_value & 0x2;
  if (bit2)
  {
    mask |= 0x100;
  }
  if (value & 0x4)
  {
    later_value = 0x800;
    mask |= later_value;
  }
  return mask;
}

#include "pe1/bounds_check.h"

void MenuInput_SetPollingPaused(int paused) {
    D_8009D0EC = paused;
}

int MenuInput_GetRepeatStep(void) {
    return D_8009D0F4;
}

static inline void AppendInputEvent(MenuInputQueuedEvent *event,
                                    MenuInputQueuedEvent *tail) {
    if (tail != 0) {
        tail->next = event;
    } else {
        if (D_8009D0E0 != 0)
            BoundsCheck_AssertStub(0x1F);
        D_8009D0E0 = event;
    }
    D_8009D0E4 = event;
}

void MenuInput_EnqueueStatusChanges(int flags) {
    int flags_reg;
    MenuInputQueuedEvent *event;
    MenuInputQueuedEvent *tail;
    int mapped;
    int released;
    int timer;
    int prev_flags;
    int repeat_reset;
    int repeat_step;
    register int type asm("$2");
    register int release_type asm("$20");

    flags_reg = flags;
    repeat_reset = 0;
    mapped = Draw_RemapStatusFlags();
    if (D_8009D0E8 != 0) {
        int inverse;
        prev_flags = D_8009D0F0;
        inverse = ~mapped;
        released = inverse & prev_flags;
        if (released != 0) {
            event = D_8009D0DC;
            release_type = 4;
            if (event != 0) {
                {
                    MenuInputQueuedEvent *next = event->next;
                    asm("" : : : "memory");
                    D_8009D0DC = next;
                }
                event->next = 0;
                tail = D_8009D0E4;
                AppendInputEvent(event, tail);
                event->payload.input.type = release_type;
                event->payload.input.flags = released;
            }
        }

        if (D_8009D0F0 != mapped) {
            D_8009D0F8 = 0x10;
        }

        timer = D_8009D0F8 - 2;
        D_8009D0F8 = timer;
        if (timer < 0) {
            if ((flags_reg != 0 && timer < -0x5A) || (timer & 3) == 0) {
                D_8009D0F0 = 0;
                repeat_reset = 1;
            }
        }

        if (D_8009D0F8 >= -0x12B) {
            repeat_step = 1;
        } else {
            repeat_step = 8;
        }
        D_8009D0F4 = repeat_step;

        {
            int previous;
            previous = D_8009D0F0;
            released = mapped & ~previous;
        }
        if (released != 0) {
            if (repeat_reset == 0 || (released & 0x40) == 0) {
                type = repeat_reset != 0 ? 2 : 1;
                event = D_8009D0DC;
                if (event != 0) {
                    register MenuInputQueuedEvent *next asm("$2");
                    int event_type;
                    event_type = type;
                    next = event->next;
                    tail = D_8009D0E4;
                    event->next = 0;
                    D_8009D0DC = next;
                    AppendInputEvent(event, tail);
                    event->payload.input.type = event_type;
                    event->payload.input.flags = released;
                }
            }
        }
        D_8009D0F0 = mapped;
    } else if (mapped == 0) {
        D_8009D0E8 = 1;
    }
}

void MenuInput_DispatchQueuedEvents(void) {
    MenuInputWidget *node;
    MenuInputQueuedEvent *event;
    MenuInputQueuedEvent *prev;
    MenuInputQueuedEvent *head;
    MenuInputQueuedEvent *free_head;
    MenuInputQueuedEvent local;
    MenuInputQueuedEvent *localp;
    int type;
    int flags;
    int handled;

    node = MenuWidget_GetCurrentNode();
    if (D_8009D0EC == 0) {
        MenuInput_EnqueueStatusChanges(node->flags);
    }

    localp = &local;
    head = D_8009D0E0;
    if (head != 0) {
        event = head;
        prev = 0;
        while (event != 0 && event->payload.input.type == 0) {
            prev = event;
            event = event->next;
        }

        if (event != 0) {
            if (prev != 0) {
                prev->next = event->next;
            } else {
                MenuInputQueuedEvent *next = event->next;
                PE1_NOP_DEP("r", next);
                D_8009D0E0 = next;
            }
            if (event == D_8009D0E4) {
                D_8009D0E4 = prev;
            }
            free_head = D_8009D0DC;
            D_8009D0DC = event;
            event->next = free_head;
            *localp = *event;
        } else {
            localp->payload.input.type = 0;
            localp->payload.input.flags = 0;
        }
    } else {
        local.payload.input.type = 0;
        local.payload.input.flags = 0;
    }

    type = local.payload.input.type;
    if (type <= 0) {
        return;
    }

    if (type >= 3 && type != 4) {
        return;
    }
    if (type < 3) {
        if (node == 0) {
            return;
        }
        do {
            flags = local.payload.input.flags;
            if (local.payload.input.type == 2) {
                flags |= 0x20000;
            }
            handled = ((MenuWidgetInputHandler)node->update)(node, flags);
            if (handled != 0) {
                return;
            }
            node = node->parent;
        } while (node != 0);
    } else {
        if ((local.payload.input.flags & 0x20) == 0) {
            return;
        }
        node = MenuWidget_GetCurrentNode();
        if (node == 0) {
            return;
        }
        do {
            handled = ((MenuWidgetInputHandler)node->update)(node, 0x10000);
            if (handled != 0) {
                return;
            }
            node = node->parent;
        } while (node != 0);
    }
}

extern int g_MenuInputActive;

int MenuInput_HasConfirm(void) {
    int ret;
    int mask;

    ret = 0;
    if (g_MenuInputActive != 0) {
        mask = Draw_RemapStatusFlags() & 0x20;
        ret = mask != 0;
    }
    return ret;
}

int MenuInput_HasNavigationRepeat(void) {
    int ret;
    int mask;

    ret = 0;
    if (g_MenuInputActive != 0) {
        mask = Draw_RemapStatusFlags() & 0x5000;
        ret = mask != 0;
    }
    return ret;
}

int g_DrawPresentEnabled;

int MenuInput_GetStatusFlags(void) {
    if (g_MenuInputActive != 0) {
        return Draw_RemapStatusFlags();
    }
    return 0;
}

void Draw_SetPresentEnabled(int arg0) {
    g_DrawPresentEnabled = arg0;
}

#define NULL ((void *)0)

DISPENV *SetDefDispEnv(DISPENV *env, int x, int y, int w, int h);
DRAWENV *SetDefDrawEnv(DRAWENV *env, int x, int y, int w, int h);
void Draw_SetFontVariant();

extern s32 g_TextCursorX;
extern s32 g_TextCursorY;
extern void *g_TextCursorStackPtr;
extern s32 g_DrawGradientBlendColor;
extern s32 g_DrawPresentImage;
extern s8 D_800A2198[];
#define D_800A2198 (D_800A2198[0])
extern s8 D_800A2199[];
#define D_800A2199 (D_800A2199[0])
extern s8 D_800A219A[];
#define D_800A219A (D_800A219A[0])
extern s8 D_800A219B[];
#define D_800A219B (D_800A219B[0])
extern s32 g_DrawBufferOtBases[];
extern s32 g_DrawBufferFrontBases[];
extern s8 D_800A2210[];
#define D_800A2210 (D_800A2210[0])
extern s8 D_800A2211[];
#define D_800A2211 (D_800A2211[0])
extern s8 D_800A2212[];
#define D_800A2212 (D_800A2212[0])
extern s8 D_800A2213[];
#define D_800A2213 (D_800A2213[0])
extern s32 D_800A2268[];
#define D_800A2268 (D_800A2268[0])
extern s32 D_800A226C[];
#define D_800A226C (D_800A226C[0])
extern s32 g_TextCursorStackBottom[];
extern s32 g_OtBufferTable[];
#define g_OtBufferTable (g_OtBufferTable[0])
extern s32 g_RenderOtBufferBaseAlt[];
#define g_RenderOtBufferBaseAlt (g_RenderOtBufferBaseAlt[0])
extern s32 g_RenderFrontBufferBase[];
#define g_RenderFrontBufferBase (g_RenderFrontBufferBase[0])
extern s32 g_RenderBackBufferBase[];
#define g_RenderBackBufferBase (g_RenderBackBufferBase[0])

void Draw_InitBuffers(void);

void Draw_InitBuffers(void) {
    u8 *bufferBase = (u8 *)g_DrawBufferFrontBases;
    DrawFrameBuffer *bufferRecord =
        (DrawFrameBuffer *)(bufferBase -
                            PE1_OFFSETOF(DrawFrameBuffer, frontBufferBase));

    bufferRecord->frontBufferBase = (u8 *)g_RenderFrontBufferBase;
    D_800A226C = g_RenderBackBufferBase;
    g_DrawBufferOtBases[0] = g_OtBufferTable;
    D_800A2268 = g_RenderOtBufferBaseAlt;
    SetDefDrawEnv(bufferBase - 0x74, 0, 0, 0x140, 0xE0);
    SetDefDrawEnv(bufferBase + 4, 0, 0xE0, 0x140, 0xE0);
    D_800A2210 = 1;
    D_800A2198 = 1;
    D_800A2199 = 0;
    D_800A219A = 0;
    D_800A219B = 0;
    D_800A2211 = 0;
    D_800A2212 = 0;
    D_800A2213 = 0;
    SetDefDispEnv(bufferBase - 0x18, 0, 0xE0, 0x140, 0xE0);
    SetDefDispEnv(bufferBase + 0x60, 0, 0, 0x140, 0xE0);
    {
        s32 color;
        register u8 *screenBase asm("$16") = bufferBase;

        /* Match debt: stop CSE rematerializing the four screen addresses.
         * A separate local keeps earlier buffer initialization unchanged. */
        asm volatile("" : "=r"(screenBase) : "0"(screenBase));
        color = 0x800000;
        ((DISPENV *)(screenBase + 0x60))->screen.y = 8;
        ((DISPENV *)(screenBase - 0x18))->screen.y = 8;
        ((DISPENV *)(screenBase + 0x60))->screen.h = 0xE0;
        ((DISPENV *)(screenBase - 0x18))->screen.h = 0xE0;
        g_TextCursorY = 0;
        g_TextCursorX = 0;
        g_TextCursorStackPtr = &g_TextCursorStackBottom;
        Draw_SetColor(color | 0x8080);
    }
    g_DrawGradientBlendColor = 0;
    Draw_SetFontVariant(0);
    g_DrawPresentImage = 0;
}

int g_DrawPresentImage;

extern int g_ActiveDrawSlot[];
extern int g_DrawBufferIndex;

void Draw_SetPresentImage(int arg0) {
    g_DrawPresentImage = arg0;
}

void Draw_SelectBuffer(void) {
    int index;
    int offset;
    DrawFrameBuffer *entry;
    u8 *value0;
    u32 *value1;

    if (g_DrawPresentEnabled != 0) {
        index = (u32)g_DrawBufferIndex < 1;
    } else {
        index = g_ActiveDrawSlot[0];
    }

    offset = ((index << 4) - index) << 3;
    g_DrawBufferIndex = index;
    entry = &D_800A2180[index];
    value0 = *(u8 **)((u8 *)g_DrawBufferFrontBases + offset);
    value1 = *(u32 **)((u8 *)g_DrawBufferOtBases + offset);
    D_8009D0FC = entry;
    g_DrawPacketArenaBase = value0;
    g_DrawPacketCursor = value0;
    D_8009D118 = value1;
    g_DrawOrderingTableEntry = value1 + 1;

    if (g_DrawPresentEnabled != 0) {
        ClearOTagR(value1, 0x1000);
    }
}

int VSync(int arg0);
void DrawSync(int arg0);
void ResetGraph(int arg0);

static inline int NormalizeSyncMode(int mode) {
    if (mode == 1) {
        mode = 0;
    }
    return mode;
}

void Draw_PresentFrame(int arg0) {
    int index;
    RECT rect;
    int mode;
    int image;
    int y;

    index = arg0;
    if (g_DrawPresentEnabled == 0) {
        return;
    }

    VSync(1);
    DrawSync(0);
    mode = index;
    index = 0xFFF;
    VSync(NormalizeSyncMode(mode));
    ResetGraph(1);
    PutDrawEnv(&D_8009D0FC->draw);
    PutDispEnv(&D_8009D0FC->display);

    image = g_DrawPresentImage;
    if (image != 0) {
        y = 0xB;
        rect.x = 0;
        if (g_DrawBufferIndex != 0) {
            y = 0xEB;
        }
        rect.y = y;
        rect.w = 0x140;
        rect.h = 0xCC;
        LoadImage(&rect, (void *)image);
    }

    DrawOTag(&D_8009D118[index]);
}
