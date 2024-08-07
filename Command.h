#ifndef COMMAND_H
#define COMMAND_H

#include "param.h"
#include <string.h>

class GenericArgument
{
public:
    virtual ~GenericArgument() = default;
};

class Argument_uint8_t : public GenericArgument
{
public:
    Argument_uint8_t() = default;

    uint8_t value;
};

class Argument_uint16_t : public GenericArgument
{
public:
    Argument_uint16_t() = default;

    uint16_t value;
};

class Argument_string : public GenericArgument
{
public:
    Argument_string() = default;

    uint8_t len = 0;
    char value[STRING_INPUT_BUFFER_LEN];
};

class GenericOption
{
public:
    bool set = false;
    char identifier;

    GenericOption(const char _identifier)
    {
        identifier = _identifier;
    }

    virtual ~GenericOption() = default;
};

class Option_uint8_t : public GenericOption
{
public:
    Option_uint8_t(uint8_t _default_value, const char _identifier)
        :
        GenericOption(_identifier)
    {
        default_value = _default_value;
        value = default_value;
    }

    uint8_t value;
    uint8_t default_value;
};

class Option_uint16_t : public GenericOption
{
public:
    Option_uint16_t(uint16_t _default_value, const char _identifier)
        :
        GenericOption(_identifier)
    {
        default_value = _default_value;
    }

    uint16_t value;
    uint16_t default_value;
};

class Option_string : public GenericOption
{
public:
    Option_string(const char* _default_value, const char _identifier)
        :
        GenericOption(_identifier)
    {
        strcpy(default_value, _default_value);
    }

    uint8_t len = 0;
    char value[STRING_INPUT_BUFFER_LEN];
    char default_value[STRING_INPUT_BUFFER_LEN];
};




class Command
{
public:
    Command() = default;
    virtual ~Command() = default;
    virtual int cmd_exec() { return 0; }
    char name[MAX_CMD_LEN] = "";

    uint8_t optc;
    GenericOption* opts[8];
    uint8_t argc;
    GenericArgument* args[8];

};

#endif