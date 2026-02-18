// common.h
#include <stdio.h>
#include <windows.h>
#include <mmreg.h>
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <avrt.h>
#include <functiondiscoverykeys_devpkey.h>
#include <mutex>

#include <audioclient.h>
#include "log.h"
#include "cleanup.h"
#include "prefs.h"
#include "loopback-capture.h"
#include "audiobuf.h"
