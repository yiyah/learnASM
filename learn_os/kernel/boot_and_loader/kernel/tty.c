#include "type.h"
#include "const.h"
#include "protect.h"
#include "process.h"
#include "proto.h"

PUBLIC void task_tty()
{
    while (1) {
        keyboard_read();
    }
}
