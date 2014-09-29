#pragma once

//#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <WinSock2.h>
#include <Windows.h>

#include <string>
#include <iostream>

#include "HttpUtil.h"

enum Port { Ping = 7, Time = 13, HTTP = 80, PROXY = 8080 };