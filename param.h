#ifndef PARAM_H
#define PARAM_H

#include <stdint.h>

#define COMMS_HALF_DUPLEX                       true


#define COMMS_ACCESSIBLE_RM_TABLE_LEN           32
#define RM_MAX_NAME_LEN                         32
#define MAX_CMD_LEN                             32
#define COMMS_SYS_MAX_NAME_LEN                  32


#define STRING_INPUT_BUFFER_LEN                 64


#define COMMS_BUFFER_LEN                        1024
#define COMMS_CMD_BUFFER_LEN                    256

// commands
#define MAX_CMDS                                256

#define MAX_OPTS                                8
#define MAX_ARGS                                8


#if COMMS_HALF_DUPLEX
#define MARKER_DELIMITER                    '$'

#define MARKER_HEADER                       'h'
#define MARKER_END                          'e'

#define MARKER_CONTROL_TRANSFER             't'
#define MARKER_ACK_ACCEPT_CONTROL_TRANSFER  'a'
#define MARKER_ACK_DATA                     'd'
#define MARKER_COMMAND                      'c'

#define HEADER_LENGTH                       4       // 2 bytes for markers, 1 byte for length, 1 byte for flags
#define HEADER_LENGTH_INNER                 2       // 1 byte for length, 1 byte for flags
#endif

#endif // PARAM_H