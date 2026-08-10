#include "include_asm.h"

#include "pe1/menu_widget.h"

int MenuWidget_GridCellIndex(MenuWidgetNode *ptr) {
    int ret;
    int x;
    int y;

    ret = -1;
    if (ptr) {
        x = ptr->cursor_x;
        if (x >= 0) {
            y = ptr->cursor_y;
            if (y >= 0) {
                ret = ptr->grid_width * y + x;
            }
        }
    }

    return ret;
}

int MenuWidget_GetCellIndex(void *w) {
    int idx = -1;
    int ret;

    if (w != 0) {
        int col = *(int *)((char *)w + 0x44);
        if (col >= 0) {
            int row = *(int *)((char *)w + 0x48);
            idx = row;
            if (row >= 0) {
                idx = (*(int *)((char *)w + 0x34) * row) + col;
            } else {
                idx = -1;
            }
        }
    }
    ret = -1;
    if ((*(int *)((char *)w + 0x74) >> idx) & 1) {
        ret = idx;
    }
    return ret;
}

extern int D_8009D10C;
extern int D_8009D12C;
extern int D_8009D124;
extern int D_8009D128;
extern char D_800A22B0[];
extern char D_800A2270[];

void BoundsCheck_AssertStub(int arg0);
int VSync(int arg0);
void Draw_AllocColorTri(int width, int height, int arg2);

void MenuWidget_DrawListRow(MenuWidgetNode *node, void (*draw_callback)(int), int row, int draw_cursor) {
    int index;
    int bit;
    int x;
    int enabled;
    int dimmed;
    int cursor_stack;
    int cursor_x;
    int cursor_y;
    int row_index;
    int (*select_callback)(int);

    index = (node->scroll_y + row) * node->x_limit;
    bit = 1 << index;

    cursor_stack = D_8009D12C;
    if ((unsigned int)cursor_stack < (unsigned int)D_800A22B0) {
        *(int *)cursor_stack = D_8009D124;
        *(int *)(cursor_stack + 4) = D_8009D128;
        D_8009D12C = cursor_stack + 8;
    } else {
        BoundsCheck_AssertStub(2);
    }

    D_8009D124 += 2;
    D_8009D128 += 2;

    for (x = 0; x < node->grid_width; x++) {
        select_callback = (int (*)(int))node->field_8C;
        enabled = 1;
        if (select_callback != 0) {
            enabled = select_callback(index);
            node->cell_mask &= ~bit;
            if (enabled != 0) {
                node->cell_mask |= bit;
            }
            index++;
            bit <<= 1;
        }

        if (enabled != 0) {
            dimmed = 0;
            if (draw_cursor != 0) {
                if (x != node->cursor_x || node->scroll_y + row != node->cursor_y) {
                    dimmed = 1;
                }
            }
            D_8009D10C = dimmed;

            if (draw_callback != 0) {
                row_index = ((node->scroll_y + row) * node->grid_width) + x;
                draw_callback(row_index);
            }

            if ((VSync(-1) & 8) != 0 && x == node->target_x && node->scroll_y + row == node->target_y) {
                D_8009D124 -= 2;
                D_8009D128 -= 2;
                Draw_AllocColorTri(node->field_3C, node->field_40, 0);
                D_8009D124 += 2;
                D_8009D128 += 2;
            }
        }

        D_8009D124 += node->field_3C;
    }

    cursor_stack = D_8009D12C;
    if ((unsigned int)D_800A2270 < (unsigned int)cursor_stack) {
        cursor_x = *(int *)(cursor_stack - 8);
        cursor_y = *(int *)(cursor_stack - 4);
        D_8009D12C = cursor_stack - 8;
        D_8009D124 = cursor_x;
        D_8009D128 = cursor_y;
    } else {
        BoundsCheck_AssertStub(3);
    }

    D_8009D128 += node->field_40;
}

typedef signed short s16;
typedef unsigned int u32;

typedef struct DrawAreaRect {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} DrawAreaRect;

extern MenuWidgetNode *D_8009D154;
extern MenuWidgetNode *D_8009D15C;
extern u32 D_8009D100;
extern u32 D_8009D104;
extern int D_8009D108;
extern u32 *D_8009D11C;
extern int D_8009D0E8;
extern int D_8009D164;
extern int D_8009D168;

void BoundsCheck_AssertStub(int arg0);
void SetDrawArea(void *packet, DrawAreaRect *rect);
void MenuWidget_EaseNodePosition(void *arg0);
int Draw_RemapStatusFlags(void);
void Draw_AllocColorTriGradient(int width, int height, int mode, int focused);

void Draw_AllocPrimWithMask(MenuWidgetNode *node) {
    MenuWidgetNode *parent;
    MenuWidgetNode *scan;
    MenuWidgetNode *child;
    int i;
    int x;
    int y;
    int w;
    int h;
    u32 oldPacket;
    u32 nextPacket;
    u32 *packet;
    u32 *ot;
    DrawAreaRect rect;

    x = 0;
    y = 0;
    parent = node;
    while (parent != 0) {
        x += parent->value_18;
        y += parent->value_1C;
        child = 0;
        scan = D_8009D154;
        while (scan != 0) {
            i = 0;
            while (i < 4) {
                if (*(MenuWidgetNode **)((char *)scan + 8 + (i * 4)) == parent) {
                    child = scan;
                    break;
                }
                i++;
            }
            if (child != 0) {
                break;
            }
            scan = scan->next;
        }
        parent = child;
    }

    w = node->field_3C * node->grid_width;
    h = node->field_40 * node->visible_rows;
    if (D_8009D108 != 0) {
        y += 0xE0;
    }
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    packet = 0;
    oldPacket = D_8009D100;
    nextPacket = oldPacket + 0xC;
    if (nextPacket < D_8009D104 + 0x4000) {
        D_8009D100 = nextPacket;
        packet = (u32 *)oldPacket;
    } else {
        BoundsCheck_AssertStub(1);
    }

    if (packet != 0) {
        SetDrawArea(packet, &rect);
    }

    ot = D_8009D11C;
    packet[0] = (packet[0] & 0xFF000000) | (*ot & 0xFFFFFF);
    *ot = (*ot & 0xFF000000) | ((u32)packet & 0xFFFFFF);
}

void MenuWidget_DrawList(MenuWidgetNode *node, void (*draw_callback)(int)) {
    MenuWidgetNode *scan;
    MenuWidgetNode *parent;
    int i;
    int row;
    int offset;
    int highlight;
    u32 *packet;
    u32 oldPacket;
    u32 nextPacket;
    u32 *ot;
    DrawAreaRect rect;
    int cursor_stack;
    int cursor_x;
    int cursor_y;

    parent = 0;
    scan = D_8009D154;
    while (scan != 0) {
        i = 0;
        while (i < 4) {
            if (*(MenuWidgetNode **)((char *)scan + 8 + (i * 4)) == node) {
                parent = scan;
                goto found_parent;
            }
            i++;
        }
        scan = scan->next;
    }

found_parent:
    D_8009D164 = node->field_3C;
    D_8009D168 = node->field_40;

    rect.x = 0;
    rect.y = 0;
    rect.w = 0x140;
    rect.h = 0xE0;
    if (D_8009D108 != 0) {
        rect.y = 0xE0;
    }

    packet = 0;
    oldPacket = D_8009D100;
    nextPacket = oldPacket + 0xC;
    if (nextPacket < D_8009D104 + 0x4000) {
        D_8009D100 = nextPacket;
        packet = (u32 *)oldPacket;
    } else {
        BoundsCheck_AssertStub(1);
    }

    if (packet != 0) {
        SetDrawArea(packet, &rect);
    }

    ot = D_8009D11C;
    packet[0] = (packet[0] & 0xFF000000) | (*ot & 0xFFFFFF);
    *ot = (*ot & 0xFF000000) | ((u32)packet & 0xFFFFFF);

    cursor_stack = D_8009D12C;
    if ((unsigned int)cursor_stack < (unsigned int)D_800A22B0) {
        *(int *)cursor_stack = D_8009D124;
        *(int *)(cursor_stack + 4) = D_8009D128;
        D_8009D12C = cursor_stack + 8;
    } else {
        BoundsCheck_AssertStub(2);
    }

    cursor_x = D_8009D124;
    cursor_y = D_8009D128 + node->field_60;
    D_8009D124 = cursor_x;
    D_8009D128 = cursor_y;

    if (node->field_60 > 0) {
        D_8009D124 = cursor_x;
        D_8009D128 = cursor_y - node->field_40;
        MenuWidget_DrawListRow(node, draw_callback, -1, parent->field_3C);
    }

    for (row = 0; row < node->visible_rows; row++) {
        MenuWidget_DrawListRow(node, draw_callback, row, parent->field_3C);
    }

    if (node->field_60 < 0) {
        MenuWidget_DrawListRow(node, draw_callback, row, parent->field_3C);
    }

    cursor_stack = D_8009D12C;
    if ((unsigned int)D_800A2270 < (unsigned int)cursor_stack) {
        cursor_x = *(int *)(cursor_stack - 8);
        cursor_y = *(int *)(cursor_stack - 4);
        D_8009D12C = cursor_stack - 8;
        D_8009D124 = cursor_x;
        D_8009D128 = cursor_y;
    } else {
        BoundsCheck_AssertStub(3);
    }

    Draw_AllocPrimWithMask(node);
    MenuWidget_EaseNodePosition(node->field_80);

    offset = node->field_60;
    if (offset != 0) {
        if (offset > 0) {
            offset -= (node->field_40 + ((unsigned int)node->field_40 >> 31)) >> 1;
            if (offset < 0) {
                offset = 0;
            }
            node->field_60 = offset;
        } else {
            offset += (node->field_40 + ((unsigned int)node->field_40 >> 31)) >> 1;
            node->field_60 = offset;
            if (offset > 0) {
                node->field_60 = 0;
            }
        }
    }

    if (node->field_60 == 0 && node->cursor_x >= 0 && node->cursor_y >= node->scroll_y &&
        node->cursor_y < node->scroll_y + node->visible_rows) {
        cursor_stack = D_8009D12C;
        if ((unsigned int)cursor_stack < (unsigned int)D_800A22B0) {
            *(int *)cursor_stack = D_8009D124;
            *(int *)(cursor_stack + 4) = D_8009D128;
            D_8009D12C = cursor_stack + 8;
        } else {
            BoundsCheck_AssertStub(2);
        }

        D_8009D124 += node->field_3C * node->cursor_x;
        D_8009D128 += node->field_40 * (node->cursor_y - node->scroll_y);

        highlight = 0;
        if ((node->field_64 & 0x80) == 0) {
            if (D_8009D15C == node) {
                highlight = 0;
                if (D_8009D0E8 != 0 && (Draw_RemapStatusFlags() & 0x20) != 0) {
                    highlight = 1;
                }
            }
        } else {
            highlight = 1;
        }

        Draw_AllocColorTriGradient(node->field_3C, node->field_40, highlight, D_8009D15C == node);

        cursor_stack = D_8009D12C;
        if ((unsigned int)D_800A2270 < (unsigned int)cursor_stack) {
            cursor_x = *(int *)(cursor_stack - 8);
            cursor_y = *(int *)(cursor_stack - 4);
            D_8009D12C = cursor_stack - 8;
            D_8009D124 = cursor_x;
            D_8009D128 = cursor_y;
        } else {
            BoundsCheck_AssertStub(3);
        }
    }
}

void MenuWidget_ClampScroll(MenuWidgetNode *ptr) {
    int y;
    int top;
    int height;
    int next;

    y = ptr->cursor_y;
    top = ptr->scroll_y;
    if (y < top) {
        ptr->scroll_y = y;
    } else {
        height = ptr->visible_rows;
        if (y >= top + height) {
            next = y - height;
            ptr->scroll_y = next + 1;
        }
    }
}

void MenuWidget_ClampCursor(MenuWidgetNode *ptr, int x, int y) {
    int limit;
    int top;
    int height;
    int next;

    if (x < 0) {
        x = 0;
    } else {
        limit = ptr->x_limit;
        if (x >= limit) {
            x = limit - 1;
        }
    }

    if (y < 0) {
        y = 0;
    } else {
        limit = ptr->y_limit;
        if (y >= limit) {
            y = limit - 1;
        }
    }

    top = ptr->scroll_y;
    ptr->cursor_x = x;
    ptr->cursor_y = y;
    if (y < top) {
        ptr->scroll_y = y;
    } else {
        height = ptr->visible_rows;
        if (y >= top + height) {
            next = y - height;
            ptr->scroll_y = next + 1;
        }
    }
}
