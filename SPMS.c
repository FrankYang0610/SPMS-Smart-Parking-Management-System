//
// COMP2432 Operating Systems Group Project
// SPMS - Smart Parking Management System
// Project Pica Magpie [https://en.wikipedia.org/wiki/Magpie]
//
// WANG Yuqi
// YANG Xikun
// LIU Siyuan
// JIN Yixiao
//

#include "input.h"
#include "output.h"
#include "utils.h"
#include "scheduler.h"
#include "analyzer.h"

#include <string.h>

typedef struct Time {
    int y, m, d;
    int hour, min;
} Time;

typedef struct Request { 
    Time start, end;

} Request;

int main() { 

}