#include <Library/UefiLib.h>

#define CHECK_STATUS(expr, message)                         \
    do {                                                    \
        Status = (expr);                                    \
        if(EFI_ERROR(Status))                               \
        {                                                   \
            Print(L"[ERROR] %a: %r\n", message, Status);    \
            goto end;                                       \
        }                                                   \
    } while(0);

#define CHECK(expr, message)        \
    do {                            \
        if(EFI_ERROR(expr))         \
        {                           \
            Print(L"[ERROR] %a\n", message); \
            goto end;               \
        }                           \
    } while(0);

#define ASSERT_EXPR(expr, msg)                                                              \
    do {                                                                                    \
        if(!(expr))                                                                         \
        {                                                                                   \
            Print(L"[ERROR] Assertion failed at %a:%d (%a).\n", __FILE__, __LINE__, msg);   \
            Status = EFI_INVALID_PARAMETER;                                                 \
            goto end;                                                                       \
        }                                                                                   \
    } while(0);

#define DO_IF(expr, _bool)      \
    do {                        \
        if(_bool)               \
            expr;               \
    } while(0);
    
#define CHECK_BIT(byte, pos) ((byte >> pos) & 1)
