//**********************************************************************`
//* This is an include file generated by Message Compiler.             *`
//*                                                                    *`
//* Copyright (c) Microsoft Corporation. All Rights Reserved.          *`
//**********************************************************************`
#pragma once
//+
// Provider okreis Event Count 3
//+
EXTERN_C __declspec(selectany) const GUID OKREIS_PROVIDER_GUID = {0xc41bb807, 0xa043, 0x4daa, {0x86, 0x3f, 0x51, 0xba, 0x8a, 0xa5, 0xe6, 0x69}};

//
// Channel
//
#define OKREIS_PROVIDER_GUID_CHANNEL_Application 0x9

//
// Event Descriptors
//
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR OKREIS_EVENT_1 = {0x1, 0x0, 0x9, 0x4, 0x0, 0x0, 0x8000000000000000};
#define OKREIS_EVENT_1_value 0x1
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR OKREIS_EVENT_2 = {0x2, 0x0, 0x9, 0x3, 0x0, 0x0, 0x8000000000000000};
#define OKREIS_EVENT_2_value 0x2
EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR OKREIS_EVENT_3 = {0x3, 0x0, 0x9, 0x2, 0x0, 0x0, 0x8000000000000000};
#define OKREIS_EVENT_3_value 0x3
#define MSG_okreis_event_1_message           0x00000001L
#define MSG_okreis_event_2_message           0x00000002L
#define MSG_okreis_event_3_message           0x00000003L
#define MSG_opcode_Info                      0x30000000L
#define MSG_level_Error                      0x50000002L
#define MSG_level_Warning                    0x50000003L
#define MSG_level_Informational              0x50000004L
#define MSG_task_None                        0x70000000L
#define MSG_channel_Application              0x90000001L
