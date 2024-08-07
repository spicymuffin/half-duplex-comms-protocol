#ifndef ROCKETMODULE_H
#define ROCKETMODULE_H

#include <stdio.h>
#include <string.h>

#include "Command.h"
#include "param.h"

class RocketModule
{
public:
    RocketModule(const char* _name, int _update_frequency) { strcpy(name, _name); update_frequency = _update_frequency; tick = 0; cmd_cnt = 0; }
    char* get_name() { return name; }
    int get_update_frequency() const { return update_frequency; }
    const uint8_t get_cmd_cnt() const { return cmd_cnt; }
    Command* get_cmd_ref(uint8_t index) { return &cmds[index]; }

    virtual const int update();

protected:
    int tick;
    int update_frequency;
    char name[RM_MAX_NAME_LEN] = "";

    Command cmds[MAX_CMDS];
    uint8_t cmd_cnt;
private:
};

#endif