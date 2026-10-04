/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G8 --use-comm-section --expand-div */
/* ASPSX_VERSION: 2.77 */
#include "pe1/battle_runtime.h"
extern u8 D_8009D25C;
extern s8 D_8009CE48;
extern s8 D_8009CE54;
extern s8 D_8009CE55;
extern s16 D_8009CE4C;
extern s32 D_8009E054, D_8009E058, D_8009E05C;
extern u16 D_8009CE58[2];
extern u16 D_8009CE5C;
extern RenderObjectEntity D_800B0CEC;
extern u16 D_800B0D88;
extern u8 D_800B0D8A;
extern u16 D_801F1F38;
extern u32 D_8009D1A0;
int Scene_InitEntityPlayer(int mode);
void Pm_StopLowerHalf(void);
int Battle_CalcAngleToTarget(void *, void *);
int rsin(int);
int rcos(int);

u8 D_8009D25C;
s8 D_8009CE48, D_8009CE54, D_8009CE55;
s16 D_8009CE4C;
u16 D_8009CE58[2], D_8009CE5C;
u8 D_8009D1D4;
int D_8009D258;

/* WIP: not selected by main.yaml; see Battle_StepAyaAction/README.md. */
s32 Battle_StepAyaAction(void)
{
  RenderObjectEntity *actionRender;
  BattleEntity *temp_a0;
  BattleEntity *var_a1;
  BattleEntity *var_v1;
  s16 temp_v0;
  BattleEntity *new_var;
  s32 temp_s0;
  s32 temp_v0_3;
  s32 temp_v1;
  s32 temp_v1_2;
  s32 var_s1;
  s8 temp_v0_2;
  u32 var_a0;
  u32 var_a0_2;
  int new_var2;
  register u8 var_v0_2 asm("$2");
  u32 *temp_a0_2;
  var_s1 = 0;
  switch (D_8009D25C)
  {
    case 0:
    {
      register s32 transfer asm("$2");
      register int rate asm("$5");
      register int mask asm("$7");
      register RenderObjectEntity *render asm("$4");
      register unsigned entityFlags asm("$3");
      Scene_InitEntityPlayer(1);
      {
        register BattleEntity *player asm("$6") = D_8009D254;
        transfer = player->posX.fixed;
        D_8009E054 = transfer;
        transfer = player->posY.fixed;
        D_8009E058 = transfer;
        transfer = player->posZ.fixed;
        D_8009E05C = transfer;
        transfer = (u16) player->rotationX;
        D_8009CE58[0] = transfer;
        rate = 30;
        D_8009CE48 = 0;
        transfer = (u16) player->facingAngle;
        D_8009CE58[1] = transfer;
        mask = ~4;
        transfer = (u16) player->rotationZ;
        D_8009CE5C = transfer;
        render = &player->renderObject;
        transfer = D_8009D2E8;
        asm volatile("" : : "r"(transfer), "r"(render), "r"(rate), "r"(mask) : "memory");
        entityFlags = player->entityFlags;
        transfer &= mask;
        entityFlags |= 0x80;
        asm volatile("" : : "r"(transfer), "r"(entityFlags) : "memory");
        D_8009D2E8 = transfer;
        player->entityFlags = entityFlags;
        Anim_SetInterpRate(render, rate);
      }
      D_8009D254->renderObject.flags_9C |= 2;
      Anim_SetInterpRate(&D_800B0CEC, 0x1E);
      D_800B0D88 |= 2;
      Pm_StopLowerHalf();
      D_8009D258 = Scene_LoadRoomAssets(0x6BU, D_8009D254);
      D_8009D25C += 1;
      break;
    }

    case 1:
      Scene_InitEntityPlayer(1);
      if (D_8009D254->renderObject.variant_visible == 0)
    {
      if (D_800B0D8A == 0)
      {
        var_v0_2 = D_8009D25C + 1;
        D_8009D25C = var_v0_2;
        break;
      }
    }
      break;

    case 2:
      if (Scene_InitEntityPlayer(1) == 0)
    {
      Entity_SetActionMode(D_8009D254, 5);
      { BattleEntity *player = D_8009D254; player->renderObject.variant_visible = 1;
      player->entityFlags |= 0x100;
      Anim_SetInterpRate(&D_8009D254->renderObject, 0x1E); }
      D_8009D254->renderObject.flags_9C |= 4;
      new_var = D_8009D254;
      Scene_LoadRoomAssets(0x6CU, new_var);
      D_8009CE4C = 0x1E;
      var_v0_2 = D_8009D25C + 1;
      D_8009D25C = var_v0_2;
      break;
    }
      break;

    case 3:
      if (D_8009CE4C == 0)
    {
      D_8009D25C += 1;
      D_8009D254->entityFlags &= ~0x100;
    }
    else
    {
      D_8009CE4C -= 1;
    }

    case 4:
      if (D_8009D254->animLastFrame == ((u16 *) (&D_8009D254->animFrame))[1])
    {
      var_v1 = D_8009D20C;
      if (var_v1 != 0)
      {
        do
        {
          if ((var_v1 != D_8009D254) && (var_v1->core != 0))
          {
            var_v1->motionX = 0;
            var_v1->motionY = 0;
            var_v1->motionZ = 0;
          }
          var_v1 = var_v1->next;
        }
        while (var_v1 != 0);
      }
      Pm_Stop(D_8009D258, D_8009D254, 0);
      var_a0 = 0x6D;
      Scene_LoadRoomAssets(var_a0, D_8009D254);
      D_8009D25C += 1;
      break;
    }

    default:
      break;

    case 5:
      Entity_SetActionMode(D_8009D254, 6);
      Anim_SetInterpRate(&D_8009D254->renderObject, 0xF);
      D_8009D25C += 1;
      D_8009D254->renderObject.flags_9C |= 2;
      break;

    case 6:
      if (D_8009D254->renderObject.variant_visible == 0)
    {
      {
        register s32 product asm("$3");
        temp_a0 = D_800BE830[D_8009D1D4].actor;
        temp_s0 = (((s32) (temp_a0->renderObject.hit_cylinder.radius * ((((EnemyCombatant *) temp_a0->core)->statusFlags2 >> 0x13) & 0x1F))) / 10) + D_8009D254->renderObject.hit_cylinder.radius;
        temp_v0 = Battle_CalcAngleToTarget(&temp_a0->renderObject, &D_8009E054);
        D_8009D254->facingAngle = temp_v0;
        product = temp_s0 * rsin((s32) temp_v0);
        asm("" : "=r"(product) : "0"(product));
        D_8009D254->posX.fixed = (D_800BE830[D_8009D1D4].actor->renderObject.target_x << 16) + (product * 16);
        product = temp_s0 * rcos((s32) D_8009D254->facingAngle);
        asm("" : "=r"(product) : "0"(product));
        D_8009D254->posZ.fixed = (D_800BE830[D_8009D1D4].actor->renderObject.target_z << 16) + (product * 16);
      }
      D_8009D254->baseX = D_8009D254->posX.fixed;
D_8009D254->baseY = D_8009D254->posY.fixed;
D_8009D254->baseZ = D_8009D254->posZ.fixed;
      Asset_Find08Alt(0x4B6, 0, (s32) D_8009D254->posX.parts.integer, (s32) D_8009D254->posY.parts.integer, (s32) D_8009D254->posZ.parts.integer);
      D_8009CE4C = 0x1E;
      var_v0_2 = D_8009D25C + 1;
      D_8009D25C = var_v0_2;
      break;
    }
      break;

    case 7:
      if (D_8009D254->animLastFrame == D_8009D254->animPrev.parts.integer)
    {
      Entity_SetActionMode(D_8009D254, 7);
      D_8009D254->animFrame = D_8009D254->animLastFrame << 0xF;
    }
      if (D_8009CE4C == 0)
    {
      D_8009D254->renderObject.variant_visible = 1;
      Anim_SetInterpRate(&D_8009D254->renderObject, 0xF);
      D_8009D25C += 1;
      D_8009D254->renderObject.flags_9C |= 4;
    }
    else
    {
      D_8009CE4C -= 1;
    }
      break;

    case 8:
      if (D_8009D254->animLastFrame == D_8009D254->animPrev.parts.integer)
    {
      { register u16 flags asm("$2") = D_8009D254->renderObject.flags_9C;
        flags |= 0x20; asm("" : "=r"(flags) : "0"(flags));
        D_8009D254->renderObject.flags_9C = flags; }
      Entity_SetActionMode(D_8009D254, ((D_8009CE48 * 2) + 8) & 0xFFFE);
      D_8009D25C += 1;
      break;
    }
      break;

    case 9:
      if (D_8009D254->animLastFrame == D_8009D254->animPrev.parts.integer)
    {
      Entity_SetActionMode(D_8009D254, ((D_8009CE48 * 2) + 9) & 0xFFFF);
      {
        register unsigned index asm("$3") = D_8009D1D4;
        register unsigned targetIndex asm("$7") = index;
        register unsigned int value asm("$2");
        register BattleEntity *target asm("$2");
        value = 2;
        D_8009CE55 = value;
        value = 1;
        new_var2 = 7;
        D_8009CE54 = value;
        target = D_800BE830[targetIndex].actor;
        temp_a0_2 = target->core;
        temp_v1 = ((*temp_a0_2) & (~0x6000)) | 0x2000;
        *temp_a0_2 = temp_v1;
        temp_v1_2 = (temp_v1 & 0xFFF3FFFF) | (((((u32) D_8009D278->action->attackWord) >> 0x14) & 3) << 0x12);
        *temp_a0_2 = temp_v1_2;
        *temp_a0_2 = (s32) ((temp_v1_2 & 0xFFFC7FFF) | ((D_8009CE55 & new_var2) << 0xF));
        if (D_8009CE48 == 2)
        {
          var_a1 = D_800BE830[targetIndex].actor;
          var_a0_2 = 0x6F;
        }
        else
          if (D_8009CE48 == 5)
        {
          var_a1 = D_800BE830[targetIndex].actor;
          var_a0_2 = 0x71;
        }
        else
        {
          var_a0_2 = 0x6E;
          if (D_8009CE48 == 6)
          {
            var_a1 = D_800BE830[targetIndex].actor;
            var_a0_2 = 0x70;
          }
          else
          {
            var_a1 = D_800BE830[targetIndex].actor;
          }
        }
        Scene_LoadRoomAssets(var_a0_2, var_a1);
      }
      temp_v0_2 = ((u8) D_8009CE48) + 1;
      D_8009CE48 = temp_v0_2;
      if (temp_v0_2 < 7)
      {
        temp_v0_3 = D_8009D1D4 & 0xFF;
        if (D_800BE830[temp_v0_3].actor == D_800BE830[temp_v0_3 + 1].actor)
        {
          { register u8 next asm("$3") = D_8009D1D4 + 1; D_8009D25C = 8;
          D_8009D1D4 = next; }
          break;
        }
      }
      D_8009D25C += 1;
      break;
    }
      break;

    case 10:
      if (D_8009D254->animLastFrame == D_8009D254->animPrev.parts.integer)
    {
      Entity_SetActionMode(D_8009D254, 7);
      if (D_8009CE48 < 7)
      {
        D_8009D25C = 5;
        D_8009D1D4 += 1;
        break;
      }
      Anim_SetInterpRate(&D_8009D254->renderObject, 15);
      D_8009D25C += 1;
      D_8009D254->renderObject.flags_9C |= 2;
      break;
    }
      break;

    case 11:
        { register BattleEntity *player asm("$8") = D_8009D254;
        register s32 transfer asm("$2");
        register s32 *saved asm("$3");
        register u16 rotationZ asm("$3");
        register s32 x asm("$6");
        register s32 y asm("$7");
        register int effect asm("$4");
        if (player->renderObject.variant_visible == 0) { effect = 0x4B6;
            saved = &D_8009E054; asm("" : "=r"(saved) : "0"(saved)); transfer = *saved; player->posX.fixed = transfer; x = player->posX.parts.integer;
            transfer = D_8009E058; player->posY.fixed = transfer; y = player->posY.parts.integer;
            transfer = D_8009E05C; player->posZ.fixed = transfer;
            transfer = *saved; player->baseX = transfer;
            transfer = D_8009E058; player->baseY = transfer;
            transfer = D_8009E05C; player->baseZ = transfer;
            transfer = D_8009CE58[0]; player->rotationX = transfer;
            transfer = D_8009CE58[1]; player->facingAngle = transfer;
            asm volatile("" : : "r"(x), "r"(y), "r"(transfer), "r"(effect) : "memory"); rotationZ = D_8009CE5C; player->rotationZ = rotationZ;
            transfer = player->posZ.parts.integer;
            Asset_Find08Alt(effect, 0, x, y, transfer);
            D_8009CE4C = 0x1E;
            var_v0_2 = D_8009D25C + 1;
            D_8009D25C = var_v0_2; break;
        }
            break;
    }
    case 12:
      if (D_8009CE4C == 0)
    {
      Entity_SetActionMode(D_8009D254, 7);
      D_8009D254->renderObject.variant_visible = 1;
      Anim_SetInterpRate(&D_8009D254->renderObject, 0x1E);
      D_801F1F38 = 1;
      D_8009CE4C = 0xF;
      D_8009D25C += 1;
      D_8009D254->renderObject.flags_9C |= 4;
      break;
    }
      D_8009CE4C -= 1;
      break;
      break;

    case 13:
      if (D_8009CE4C == 0)
    {
      if (D_8009D254->animLastFrame == D_8009D254->animPrev.parts.integer)
      {
        Entity_SetActionMode(D_8009D254, 4);
        D_8009D25C += 1;
        break;
      }
      break;
    }
      D_8009CE4C -= 1;
      break;

    case 14:
      if (D_8009D254->animLastFrame == ((u16 *) (&D_8009D254->animFrame))[1])
    {
      D_8009D254->entityFlags |= 0x100;
      Anim_SetInterpRate(&D_8009D254->renderObject, 0xF);
      var_a0 = 0x72;
      D_8009D254->renderObject.flags_9C |= 2;
      Scene_LoadRoomAssets(var_a0, D_8009D254);
      D_8009D25C += 1;
      break;
    }
      break;

    case 15:
      if (D_8009D254->renderObject.variant_visible == 0)
    {
      if (Scene_InitEntityPlayer(0) == 0)
      {
        Entity_SetActionMode(D_8009D254, (s32) D_8009D278->actionMode12);
        { register int mask asm("$3") = ~0x100;
          register int rate asm("$5") = 30;
          register BattleEntity *player asm("$6") = D_8009D254;
          register RenderObjectEntity *render asm("$4");
          register unsigned flags asm("$2");
          D_8009D2E8 |= 4;
          player->renderObject.variant_visible = 1;
          render = &D_8009D254->renderObject;
          flags = player->entityFlags;
          flags &= mask;
          asm("" : "=r"(flags) : "0"(flags));
          mask = ~0x80; flags &= mask;
          player->entityFlags = flags;
          Anim_SetInterpRate(render, rate);
        }
        D_8009D254->renderObject.flags_9C |= 4;
        {
          register u8 *visible asm("$6") = &D_800B0D8A;
          asm("" : "=r"(visible) : "0"(visible));
          *visible = 1;
          Anim_SetInterpRate((RenderObjectEntity *) (visible - 0x9E), 0x1E);
        }
        { register unsigned next asm("$3") = D_8009D25C + 1;
        D_8009CE4C = 0xF;
        D_800B0D88 |= 4;
        D_8009D25C = next; }
        break;
      }
    }
      break;

    case 16:
      if (D_8009CE4C == 0)
    {
      {
        register int mask asm("$4") = ~0x100;
        register unsigned flags asm("$3") = D_8009D1A0;
        register u8 next asm("$2") = D_8009D1D4 + 1;
        D_8009D1A0 = flags & mask;
        D_8009D1D4 = next;
      }
      var_s1 = 1;
    }
    else
    {
      D_8009CE4C -= 1;
      break;
    }
      break;

  }

  return var_s1;
}
