#define CHECK_STATUS(expr, message)                         \
    do {                                                    \
        Status = (expr);                                    \
        if(EFI_ERROR(Status))                               \
        {                                                   \
            Print(L"[ERROR] %s: %r\n", message, Status);    \
            goto end;                                       \
        }                                                   \
    } while(0);

#define CHECK(expr, message)        \
    do {                            \
        if(EFI_ERROR(expr))         \
        {                           \
            Print(L"[ERROR] %s\n"); \
            goto end;               \
        }                           \
    } while(0);

#define ASSERT_EXPR(expr, msg)                                                              \
    do {                                                                                    \
        if(!(expr))                                                                         \
        {                                                                                   \
            Print(L"[ERROR] Assertion failed at %s:%d (%s).\n", __FILE__, __LINE__, msg);   \
            Status = EFI_NOT_FOUND;                                                         \
            goto end;                                                                       \
        }                                                                                   \
    } while(0);
