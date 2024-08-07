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

    // half duplex communication protocol:

    // ------------------------------
    //            ground
    // ------------------------------

    // type - control transfer

    // control_transfer
    // $h<data segment length in bytes (8bit)><flags (8bit)>$t<control transfer number (16bit)>$e

    // type - acks

    // ack_accept_control_transfer
    // $h<data segment length in bytes (8bit)><flags (8bit)>$a<control transfer number (16bit)>$e

    // ack_data
    // $h<data segment length in bytes (8bit)><flags (8bit)>$d<control transfer number (16bit)>$e

    // type - command

    // command
    // $h<data segment length in bytes (8bit)><flags (8bit)>$c<command args>$e


    // ------------------------------
    //            rocket
    // ------------------------------

    // type - acks

    // ack_control_transfer
    // $h<data segment length in bytes (8bit)><flags (8bit)>$b<control transfer number (16bit)>$e

    // type - data

    // data_log (infer log message length from header)
    // $h<data segment length in bytes (8bit)><flags (8bit)>$l<log message>$e

    // data_command_completion
    // $h<data segment length in bytes (8bit)><flags (8bit)>$r<request number (16 bit)><status code (8bit)>$e

    // data_generic
    // $h<data segment length in bytes (8bit)><flags (8bit)>$d<request number (16 bit)><packet number (8bit)><data>$e


    // only states that are relevant to the rocket are defined here
    enum communication_state
    {
        awaiting_control_transfer,
        awaiting_ack_accept_control_transfer,
        awaiting_ack_data,
    };

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

    communication_state cs = awaiting_control_transfer;

    validation_state vs = in_undefined_expecting_marker_delimiter;

private:

    // ------------------------------
    //             meta
    // ------------------------------

    int update_frequency;                       // frequency at which the comms module is updated
    RocketModule* accessible;                   // modules the interfaces of which are exposed to this comms module

    Command cmds[MAX_CMDS];                     // commands that are executable by this comms system
    char name[COMMS_SYS_MAX_NAME_LEN] = "";     // name of the comms system

    // ------------------------------
    //       serial read related
    // ------------------------------

    char available = ' ';                       // available byte for reading

    char buffer[COMMS_BUFFER_LEN];              // buffer for incoming data
    uint16_t buffer_written_cnt = 0;            // number of bytes written to the buffer
    uint16_t buffer_write_ptr = 0;              // write pointer for the buffer

    // ------------------------------
    //   message valiation related
    // ------------------------------

    char current;                               // current byte being processed
    uint16_t buffer_validation_ptr = 0;         // pointer to the byte being validated

    uint8_t header_read_count = 0;              // number of bytes read in the header
    uint16_t header_start_ptr = 0;              // start pointer of the header

    uint8_t data_segment_read_count = 0;        // number of bytes read in the data segment
    uint16_t data_segment_start_ptr = 0;        // start pointer of the data segment

    uint8_t data_segment_length = 0;            // length of the data segment

    bool marker_delimiter_found = false;        // whether the marker delimiter has been found
    uint16_t last_delimiter_ptr = 0;            // pointer to the last delimiter found


    // ------------------------------
    //       command related
    // ------------------------------

    // command execution related
    uint8_t accessible_rm_cnt;                                              // number of accessible rocket modules
    RocketModule* accessible_rm_table[COMMS_ACCESSIBLE_RM_TABLE_LEN];       // table of accessible rocket modules

    // command parsing+validation buffer
    uint16_t cmd_buffer_ptr;                // pointer to the current byte being processed in the command buffer
    char cmd_buffer[COMMS_CMD_BUFFER_LEN];  // buffer for validated data segment which contains a command

    // command validation functions
    int check_rocket_module_index_validity(uint8_t index);
    int check_cmdcode_index_validity(uint8_t rm_index, uint8_t cmdcode);
    int check_option_identifier_validity(const Command* command_ref, char identifier);
    int check_argument_index_validity(const Command* command_ref, uint8_t argument_index);

    // command parsing
    int parse_cmd_buffer(char* a, uint16_t len);

    int parse_data_segment();
    uint16_t buffer_parse_ptr = 0;
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