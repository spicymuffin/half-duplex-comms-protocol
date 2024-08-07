#ifndef COMMUNICATIONSYSTEM_H
#define COMMUNICATIONSYSTEM_H

#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "param.h"

#include "RocketModule.h"

// comms module spec:
class CommunicationSystem
{
public:
    CommunicationSystem(const char* _name, int _update_frequency, RocketModule* _accessible);
    void update();
    int parse_cmd_buffer(char* a, uint16_t len);

    #if COMMS_FULL_DUPLEX
    enum validation_state
    {
        in_undefined_expecting_marker_delimiter,

        in_marker_expecting_header_identifier,
        in_header,
        in_header_expecting_marker_delimiter,

        in_marker_expecting_data_segment_identifier,
        in_data_segment,
        in_data_segment_expecting_marker_delimiter,

        in_marker_expecting_end_identifier,
    };

    validation_state vs = in_undefined_expecting_marker_delimiter;
    #endif

private:
    int check_header_validity();

    int check_rocket_module_index_validity(uint8_t index);
    int check_cmdcode_index_validity(uint8_t rm_index, uint8_t cmdcode);
    int check_option_identifier_validity(const Command* command_ref, char identifier);
    int check_argument_index_validity(const Command* command_ref, uint8_t argument_index);
    int parse_data_segment();

    void buffer_travel_left(uint16_t dst);
    void buffer_travel_right(uint16_t dst);


    uint32_t hash_cstring(const char* str);


    RocketModule* accessible_rm_table[COMMS_ACCESSIBLE_RM_TABLE_LEN];
    uint8_t accessible_rm_cnt;


    uint16_t cmd_buffer_ptr;
    char cmd_buffer[COMMS_CMD_BUFFER_LEN];


    uint16_t buffer_parse_ptr = 0;
    uint16_t buffer_write_ptr = 0;
    uint16_t buffer_written_cnt = 0;

    // char or uint8_t idc
    char buffer[COMMS_BUFFER_LEN];

    char name[COMMS_SYS_MAX_NAME_LEN] = ""; // name of the comms system
    int update_frequency;
    RocketModule* accessible; // modules the interfaces of which are exposed to this comms module.

    // meta command related
    Command cmds[MAX_CMDS];

    // acks


    // validation
    char available = ' ';
    char current;

    uint8_t header_read_count = 0;
    uint8_t data_segment_read_count = 0;

    uint16_t data_segment_length_byte_ptr = 0;
    uint8_t data_segment_length = 0;

    uint16_t buffer_validation_ptr = 0;

    uint16_t data_segment_start_ptr = 0;

    bool marker_delimiter_found = false;
    uint16_t last_delimiter_ptr = 0;
};

class cmd_ping : public Command
{
public:
    cmd_ping()
    {
        strcpy(name, "ping");
        optc = 0;
        argc = 0;
    }

    int cmd_exec() override
    {
        return 0;
    }
};

class cmd_echo : public Command
{
public:
    Argument_string arg1 = Argument_string();

    cmd_echo()
    {
        strcpy(name, "echo");
        optc = 0;
        argc = 1;

        args[0] = &arg1;
    }

    int cmd_exec() override
    {
        return 0;
    }
};

class cmd_debug : public Command
{
public:
    Option_uint8_t opt1 = Option_uint8_t(1, '1');
    Option_uint16_t opt2 = Option_uint16_t(1, '2');

    Argument_uint8_t arg1 = Argument_uint8_t();
    Argument_string arg2 = Argument_string();

    cmd_debug()
    {
        strcpy(name, "debug");
        optc = 2;
        argc = 2;

        opts[0] = &opt1;
        opts[1] = &opt2;

        args[0] = &arg1;
        args[1] = &arg2;
    }

    int cmd_exec() override
    {
        return 0;
    }
};

#endif