#include <3ds.h>
#include "memory.h"
#include "service_manager.h"
#include "utils.h"
#include "MyThread.h"

int main(void)
{

    // new 3dsか確認
    s64 out;
    if(svcGetSystemInfo(&out, 0x10001, 0) != 0)
        goto exit;

    // init

    //

    Draw_Init();

    // QTMの確認
    if(){
        
        // QTMを無効化
        rpDoQTMPatchAndToggle();
            
    }else{
        // QTMを有効化
        rpDoQTMPatchAndToggle();
    }

deinit:
    

exlt:

    return 0;
}
