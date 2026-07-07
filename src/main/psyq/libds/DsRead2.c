/* MASPSX_FLAGS: --stack-return-delay */

extern int g_DsStreamNoLocFlag;

int DsDataCallback(void *arg0);
int DsSyncCallback(void *arg0);
int Render_BuildParticleFrame(int arg0, void *arg1, int arg2, void *arg3, int arg4);
void data_ready_callback(void);
void CdRom_BreakSyncCallback(void);

int DsRead2(void *pos, int mode) {
    void *saved_data;
    register int saved_mode asm("$16");
    void *saved_sync;
    register void *saved_pos asm("$18");
    int ret;

    saved_pos = pos;
    saved_mode = mode;
    if (saved_mode & 0x100) {
        asm volatile(
            ".set\tnoat\n\t"
            ".set\tnoreorder\n\t"
            "andi\t$2,%0,0x20\n\t"
            "beqz\t$2,1f\n\t"
            "addiu\t$2,$zero,1\n\t"
            "lui\t$1,%%hi(g_DsStreamNoLocFlag)\n\t"
            "j\t2f\n\t"
            "sw\t$zero,%%lo(g_DsStreamNoLocFlag)($1)\n"
            "1:\n\t"
            "lui\t$1,%%hi(g_DsStreamNoLocFlag)\n\t"
            "sw\t$2,%%lo(g_DsStreamNoLocFlag)($1)\n"
            "2:\n\t"
            ".set\treorder\n\t"
            ".set\tat"
            :
            : "r"(saved_mode)
            : "$2", "$1", "memory");
        {
            register void *callback_result asm("$2");

            /* Match note: data_ready_callback low half is in the DsDataCallback delay slot. */
            asm volatile(
                ".set\tnoreorder\n\t"
                "lui\t$4,%%hi(data_ready_callback)\n\t"
                "jal\tDsDataCallback\n\t"
                "addiu\t$4,$4,%%lo(data_ready_callback)\n\t"
                ".set\treorder"
                : "=r"(callback_result)
                :
                : "$4", "$31", "memory");
            saved_data = callback_result;
        }
        saved_sync = (void *)DsSyncCallback(CdRom_BreakSyncCallback);
        ret = Render_BuildParticleFrame(saved_mode & 0xFF, saved_pos, 0x1B, 0, -1);
        if (ret == 0) {
            DsDataCallback(saved_data);
            DsSyncCallback(saved_sync);
            return 0;
        }
        return ret;
    }

    return Render_BuildParticleFrame(saved_mode & 0xFF, saved_pos, 0x1B, 0, -1);
}
