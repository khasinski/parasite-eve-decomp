int Pad_IsMenuConfirmAvailable(void);

int Battle_IsInputAllowedWrapped(void)
{
    return (signed char)Pad_IsMenuConfirmAvailable();
}
