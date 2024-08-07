#include "CommunicationSystem.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "RocketModule.h"
#include "param.h"

CommunicationSystem::CommunicationSystem(const char* _name, int _updateFrequency, RocketModule* _accessible)
{
    strcpy(name, _name);
    update_frequency = _updateFrequency;
    accessible = _accessible;
}

int CommunicationSystem::check_header_validity()
{

}

void CommunicationSystem::buffer_travel_left(uint16_t dst)
{

}

void CommunicationSystem::buffer_travel_right(uint16_t dst)
{

}

#if COMMS_FULL_DUPLEX
// full duplex communication protocol

// // common ack format
// // GROUND OR SKY message receival confirmation format. literally any message should be acked
// $h<packet length in bytes (8bit)><flags (8bit)>$a<request status code (8bit)><received packet's hashsum (32bit)>$e


// // command request formats
// // GROUND command format. rocket needs to start execution of this command
// $h<packet length in bytes (8bit)><flags (8bit)>$c<request id (16bit)><command (rocket module index, command index, options, args)>$e


// // command completion format. if the command can be completed fast, we could make it so receival confirmation is skipped and completion message is sent right away
// // SKY command execution completion format. rocket has completed the execution of the command. completion status code is sent back
// $h<packet length in bytes (8bit)><flags (8bit)>$r<request id (16bit)><completion status code (8bit)><data segment (depends on command)>$e


// // data transfer formats
// // the data transfer initiator sends an init metadata message
// // GROUND OR SKY data transfer init metadata format
// $h<packet length in bytes (8bit)><flags (8bit)>$m<total number of packets in transfer(8bit)>$e
// // GROUND OR SKY data transfer packet
// $h<packet length in bytes (8bit)><flags (8bit)>$d<packet index (8bit)><packet data>$e
// // GROUND OR SKY data transfer finalization message
// $h<packet length in bytes (8bit)><flags (8bit)>$f$e
// // GROUND OR SKY data transfer finalization reply
// $h<packet length in bytes (8bit)><flags (8bit)>$i<status code (8bit, ok/abort/missing)><missing packet indices (each 8 bit)>$e

bool is_valid_data_segment_identifier(char marker)
{
    return
        marker == MARKER_ACK ||
        marker == MARKER_COMMAND ||
        marker == MARKER_RESPONSE ||
        marker == MARKER_METADATA ||
        marker == MARKER_DATA ||
        marker == MARKER_FINALIZATION ||
        marker == MARKER_FINALIZATION_REPLY;
}

bool is_valid_any_marker(char marker)
{
    return
        marker == MARKER_HEADER ||
        marker == MARKER_END ||
        marker == MARKER_ACK ||
        marker == MARKER_COMMAND ||
        marker == MARKER_RESPONSE ||
        marker == MARKER_METADATA ||
        marker == MARKER_DATA ||
        marker == MARKER_FINALIZATION ||
        marker == MARKER_FINALIZATION_REPLY;
}

void CommunicationSystem::update()
{
    // read from the serial buffer
    for (int i = 0; i < 10; i++)
    {
        buffer[buffer_write_ptr] = available;

        /// TODO: there is a possibility that the buffer will overflow
        buffer_written_cnt++;
        buffer_write_ptr = (buffer_write_ptr + 1) % COMMS_BUFFER_LEN;

        // validate message first for integrity
        // ack and higher level logic will be handled elsewhere

        for (buffer_validation_ptr = buffer_write_ptr - 1; buffer_validation_ptr != buffer_write_ptr; buffer_validation_ptr = (buffer_validation_ptr + 1) % COMMS_BUFFER_LEN)
        {
            current = buffer[buffer_validation_ptr];

            if (current == MARKER_DELIMITER && !marker_delimiter_found && vs != in_undefined_expecting_marker_delimiter)
            {
                marker_delimiter_found = true;
                last_delimiter_ptr = buffer_validation_ptr;
            }

            switch (vs)
            {
            case in_undefined_expecting_marker_delimiter:

                if (current == MARKER_DELIMITER)
                {
                    // advance to the next part of the message
                    vs = in_marker_expecting_header_identifier;
                }
                // else do nothing, remain in_undefined_expecting_marker_delimiter
                break;

            case in_marker_expecting_header_identifier:

                if (current == MARKER_HEADER)
                {
                    // advance to the next part of the message
                    vs = in_header;
                }
                else
                {
                    vs = in_undefined_expecting_marker_delimiter;
                    if (marker_delimiter_found)
                    {
                        buffer_validation_ptr = last_delimiter_ptr;
                        marker_delimiter_found = false;
                    }
                }
                break;

            case in_header:

                header_read_count++;
                if (header_read_count == 1)
                {
                    data_segment_length = (uint8_t)current;
                }

                if (header_read_count == HEADER_LENGTH_INNER)
                {
                    vs = in_header_expecting_marker_delimiter;
                    header_read_count = 0;
                }
                break;

            case in_header_expecting_marker_delimiter:

                if (current == MARKER_DELIMITER)
                {
                    vs = in_marker_expecting_data_segment_identifier;
                }
                else
                {
                    vs = in_undefined_expecting_marker_delimiter;
                    if (marker_delimiter_found)
                    {
                        buffer_validation_ptr = last_delimiter_ptr;
                        marker_delimiter_found = false;
                    }
                }
                break;

            case in_marker_expecting_data_segment_identifier:

                if (is_valid_data_segment_identifier(current))
                {
                    vs = in_data_segment;
                }
                else
                {
                    vs = in_undefined_expecting_marker_delimiter;
                    if (marker_delimiter_found)
                    {
                        buffer_validation_ptr = last_delimiter_ptr;
                        marker_delimiter_found = false;
                    }
                }
                break;

            case in_data_segment:

                data_segment_read_count++;

                if (data_segment_read_count == 1)
                {
                    data_segment_start_ptr = buffer_validation_ptr;
                }

                if (data_segment_read_count == data_segment_length)
                {
                    vs = in_data_segment_expecting_marker_delimiter;
                }
                break;

            case in_data_segment_expecting_marker_delimiter:

                if (current == MARKER_DELIMITER)
                {
                    vs = in_marker_expecting_end_identifier;
                }
                else
                {
                    vs = in_undefined_expecting_marker_delimiter;
                    if (marker_delimiter_found)
                    {
                        buffer_validation_ptr = last_delimiter_ptr;
                        marker_delimiter_found = false;
                    }
                }
                break;

            case in_marker_expecting_end_identifier:

                if (current == MARKER_END)
                {
                    // message is valid
                    // parse the message



                    // reset the validation state
                    vs = in_undefined_expecting_marker_delimiter;
                }
                break;

            default:
                break;
            }
        }
    }
}
#endif

#if COMMS_HALF_DUPLEX
void CommunicationSystem::update()
{
    return;
}
#endif

int CommunicationSystem::parse_data_segment()
{

}

int CommunicationSystem::check_rocket_module_index_validity(uint8_t index)
{
    if (index >= accessible_rm_cnt)
    {
        return -1; // rocket module index out of bounds
    }

    return 0;
}

// INVARIANT: rocket module at rm_index is a valid rocket module
int CommunicationSystem::check_cmdcode_index_validity(uint8_t rm_index, uint8_t cmdcode)
{
    uint8_t cmd_cnt = accessible_rm_table[rm_index]->get_cmd_cnt();

    if (cmdcode >= cmd_cnt)
    {
        return -1;
    }

    return 0;
}

// INVARIANT: rocket module at rm_index and its cmdcode are all valid
int CommunicationSystem::check_option_identifier_validity(const Command* command_ref, char identifier)
{
    for (uint8_t i; i < command_ref->optc; i++)
    {
        if (command_ref->opts[i]->identifier == identifier)
        {
            return i;
        }
    }

    return -1;
}

// INVARIANT: rocket module at rm_index and its cmdcode are all valid
int CommunicationSystem::check_argument_index_validity(const Command* command_ref, uint8_t argument_index)
{
    if (argument_index < command_ref->argc)
    {
        return 0;
    }
    return -1;
}


uint16_t assemble_uint16_from_chars(const char* ptr)
{
    uint16_t high_byte = static_cast<uint8_t>(*(ptr + 0));
    uint16_t low_byte = static_cast<uint8_t>(*(ptr + 1));
    return (high_byte << 8) | low_byte;
}

int CommunicationSystem::parse_cmd_buffer(char* cmd_buffer, uint16_t len)
{
    int status_code = 0;
    uint8_t rm_index;
    uint8_t cmdcode;
    uint8_t option_index;

    // BYTE 1: rocket module index (uint8_t)
    status_code = check_rocket_module_index_validity((uint8_t)(cmd_buffer[0]));

    // [10] ERROR: rocket module not found
    if (status_code != 0) { return 10; }

    rm_index = (uint8_t)(cmd_buffer[0]);

    // BYTE 2: cmdcode index (uint8_t)
    status_code = check_cmdcode_index_validity(rm_index, (uint8_t)(cmd_buffer[1]));

    // [20] ERROR: cmdcode not found
    if (status_code != 0) { return 20; }

    cmdcode = (uint8_t)(cmd_buffer[1]);

    // command reference
    Command* command_ref = accessible_rm_table[rm_index]->get_cmd_ref(cmdcode);

    // BYTES 3 <-> COMMS_CMD_BUFFER_LEN: options, arguments
    uint16_t i = 2;
    uint8_t j = 0;

    // scroll options
    for (j = 0; j < MAX_OPTS; j++)
    {
        // [1] ERROR: comms buffer exhausted
        if (i >= len) return 1;
        if (cmd_buffer[i] != '-')
        {
            break;
        }

        i++;

        // [1] ERROR: comms buffer exhausted
        if (i >= len) return 1;
        status_code = check_option_identifier_validity(command_ref, cmd_buffer[i]);

        if (status_code >= 0)
        {
            option_index = (uint8_t)status_code;
        }

        // move to option value
        i++;
        /// TODO: error checking?
        if (Option_uint8_t* ref_uint8_t = dynamic_cast<Option_uint8_t*>(command_ref->opts[option_index]))
        {
            // [1] ERROR: comms buffer exhausted
            if (i >= len) return 1;
            ref_uint8_t->value = (uint8_t)cmd_buffer[i];
            i += 1;
        }
        else if (Option_uint16_t* ref_uint16_t = dynamic_cast<Option_uint16_t*>(command_ref->opts[option_index]))
        {
            // [1] ERROR: comms buffer exhausted
            if (i + 1 >= len) return 1;
            ref_uint16_t->value = assemble_uint16_from_chars(cmd_buffer + i);
            i += 2;
        }
        else if (Option_string* ref_string = dynamic_cast<Option_string*>(command_ref->opts[option_index]))
        {
            // [1] ERROR: comms buffer exhausted
            if (i + 1 >= len) return 1;
            uint8_t string_len = (uint8_t)cmd_buffer[i];
            uint8_t k;

            for (k = 0; k < STRING_INPUT_BUFFER_LEN - 1; k++)
            {
                if (k == string_len)
                {
                    ref_string->value[k] = '\0';
                    break;
                }
                // [1] ERROR: comms buffer exhausted
                if (i >= len) return 1;
                ref_string->value[k] = cmd_buffer[i++];
            }

            if (k == STRING_INPUT_BUFFER_LEN)
            {
                // [101] ERROR: string too long
                return 101;
            }
        }
        else
        {
            // [228] ERROR: developer error
            return 228;
        }

        command_ref->opts[option_index]->set = true;

        i++;
    }

    if (j == MAX_OPTS)
    {
        // [31] ERROR: max options reached
        return 31;
    }

    // scroll arguments
    for (j = 0; j < MAX_ARGS; j++)
    {
        // [1] ERROR: comms buffer exhausted
        if (i >= len) return 1;
        if (cmd_buffer[i] == '\0')
        {
            break;
        }

        if (j + 1 >= command_ref->argc)
        {
            break;
        }

        if (Option_uint8_t* ref_uint8_t = dynamic_cast<Option_uint8_t*>(command_ref->args[j]))
        {
            // [1] ERROR: comms buffer exhausted
            if (i >= len) return 1;
            ref_uint8_t->value = (uint8_t)cmd_buffer[i];
            i += 1;
        }
        else if (Option_uint16_t* ref_uint16_t = dynamic_cast<Option_uint16_t*>(command_ref->args[j]))
        {
            // [1] ERROR: comms buffer exhausted
            if (i + 1 >= len) return 1;
            ref_uint16_t->value = assemble_uint16_from_chars(cmd_buffer + i);
            i += 2;
        }
        else if (Option_string* ref_string = dynamic_cast<Option_string*>(command_ref->args[j]))
        {
            // [1] ERROR: comms buffer exhausted
            if (i + 1 >= len) return 1;
            uint8_t string_len = (uint8_t)cmd_buffer[i];
            uint8_t k;

            for (k = 0; k < STRING_INPUT_BUFFER_LEN - 1; k++)
            {
                if (k == string_len)
                {
                    ref_string->value[k] = '\0';
                    break;
                }
                // [1] ERROR: comms buffer exhausted
                if (i >= len) return 1;
                ref_string->value[k] = cmd_buffer[i++];
            }

            if (k == STRING_INPUT_BUFFER_LEN)
            {
                // [101] ERROR: string too long
                return 101;
            }
        }
        else
        {
            // [228] ERROR: developer error
            return 228;
        }

        i++;
    }

    if (j + 1 < command_ref->argc)
    {
        // [41] ERROR: not enough arguments
        return 41;
    }

    else if (j + 1 > command_ref->argc)
    {
        // [42] ERROR: too many arguments
        return 42;
    }

    command_ref->cmd_exec();

    return 0;
}