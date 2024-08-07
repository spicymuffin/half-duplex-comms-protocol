#ifndef PARAM_H
#define PARAM_H

#include <stdint.h>

#define RM_MAX_NAME_LEN                         32
#define MAX_CMDS                                32
#define STRING_INPUT_BUFFER_LEN                 32
#define COMMS_ACCESSIBLE_RM_TABLE_LEN           32
#define MAX_CMD_LEN                             32
#define COMMS_SYS_MAX_NAME_LEN                  32

#define COMMS_CMD_BUFFER_LEN                256
#define COMMS_BUFFER_LEN                    1024

// commands
#define MAX_CMD_LEN                         16
#define MAX_CMDS                            8

#define MAX_OPTS                            8
#define MAX_ARGS                            8

#define STRING_INPUT_BUFFER_LEN             16

#define COMMS_HALF_DUPLEX                   true
#define COMMS_FULL_DUPLEX                   false


#if COMMS_FULL_DUPLEX

#define MARKER_DELIMITER                    '$'

#define MARKER_HEADER                       'h'
#define MARKER_END                          'e'

#define MARKER_ACK                          'a'
#define MARKER_COMMAND                      'c'
#define MARKER_RESPONSE                     'r'
#define MARKER_METADATA                     'm'
#define MARKER_DATA                         'd'
#define MARKER_FINALIZATION                 'f'
#define MARKER_FINALIZATION_REPLY           'i'

#define HEADER_LENGTH                       6       // 4 bytes for markers, 1 byte for length, 1 byte for flags
#define HEADER_LENGTH_INNER                 2       // 1 byte for length, 1 byte for flags

#endif

#if COMMS_HALF_DUPLEX

#endif

#endif // PARAM_H