int Battle_IsActive(void);

int Battle_IsActiveWrapped(void)
{
    return (signed char)Battle_IsActive();
}
