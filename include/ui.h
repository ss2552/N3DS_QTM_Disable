int showMsgVerbose(const char *file_name, int line_number, const char *func_name, const char *fmt, ...) __attribute__((format(printf, 4, 5)));

#define showMsg(fmt, ...) \
    showMsgVerbose(__FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__)

#define showDbg(fmt, ...)            \
    do                               \
    {                                \
        showMsg(fmt, ##__VA_ARGS__); \
    } while (0)
