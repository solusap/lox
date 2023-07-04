#define UNW_LOCAL_ONLY
#include <libunwind.h>
#include <fmt/core.h>
#include <fmt/color.h>

void backtrace() {
    unw_cursor_t cursor;
    unw_context_t context;

    // Initialize cursor to current frame for local unwinding.
    unw_getcontext(&context);
    unw_init_local(&cursor, &context);

    // Unwind frames one by one, going up the frame stack.
    int index = 0;
    while (unw_step(&cursor) > 0) {
        unw_word_t offset, pc;
        unw_get_reg(&cursor, UNW_REG_IP, &pc);
        if (pc == 0) {
            break;
        }

        // 通过函数指针获取函数对应的名称
        char sym[256];
        if (unw_get_proc_name(&cursor, sym, sizeof(sym), &offset) == 0) {
            fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "line={}||traceback=({}+{:#x})[{:#x}]\n", index, sym, offset, pc);
        } else {
            fmt::print(" -- error: unable to obtain symbol name for this frame\n");
        }
        ++index;
    }
}



std::string backtrace2String() {
    unw_cursor_t cursor;
    unw_context_t context;

    // Initialize cursor to current frame for local unwinding.
    unw_getcontext(&context);
    unw_init_local(&cursor, &context);

    // Unwind frames one by one, going up the frame stack.
    int index = 0;
    std::string ret;
    while (unw_step(&cursor) > 0) {
        unw_word_t offset, pc;
        unw_get_reg(&cursor, UNW_REG_IP, &pc);
        if (pc == 0) {
            break;
        }

        // 通过函数指针获取函数对应的名称
        char sym[256];
        if (unw_get_proc_name(&cursor, sym, sizeof(sym), &offset) == 0) {
            ret += fmt::format("line={}||traceback=({}+{:#x})[{:#x}]\n", index, sym, offset, pc);
        } else {
            fmt::print(" -- error: unable to obtain symbol name for this frame\n");
        }
        ++index;
    }
    return ret;
}