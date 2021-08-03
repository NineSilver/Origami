#define CHECK_STATUS(expr, message)                     \
    do {                                                \
        Status = expr;                                  \
        if(EFI_ERROR(Status))                           \
        {                                               \
            Print(L"[ERROR] %s: %r", message, Status);  \
            goto end;                                   \
        }                                               \
    } while(0)

#define CHECK(expr, message)       \
    do {                           \
        if(EFI_ERROR(expr))        \
        {                          \
            Print(L"[ERROR] %s");  \
            goto end;              \
        }                          \
    } while(0)
