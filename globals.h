#pragma once
#include "includes.h"
#include <math.h>

namespace globals
{
    //monitor resolution
    int res_x = 1920;
    int res_y = 1080;

    //modules
    DWORD client;
    DWORD engine;

    //process info
    DWORD process_id;
    HANDLE _handle;
    bool b_unloading = false;
    bool b_run_features = false;
}