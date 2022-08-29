/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2022 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <rtems/telnetd.h>
#include <lwip/dhcp.h>
#include <arch/sys_arch.h>

#include <tmacros.h>

#include <netstart.h>

#include "tcp_echoserver/tcp_echoserver.h"

const char rtems_test_name[] = "TCP ECHO SERVER";

struct netif net_interface;

rtems_shell_env_t env;

#define print_ip( tag, ip ) \
  printf( \
  "%s: %" PRId32 ".%" PRId32 ".%" PRId32 ".%" PRId32 "\n", \
  tag, \
  ( ntohl( ip.addr ) >> 24 ) & 0xff, \
  ( ntohl( ip.addr ) >> 16 ) & 0xff, \
  ( ntohl( ip.addr ) >> 8 ) & 0xff, \
  ntohl( ip.addr ) & 0xff \
  );

static int shell_main_netinfo(
  int    argc,
  char **argv
)
{
  print_ip( "IP", net_interface.ip_addr.u_addr.ip4 );
  print_ip( "Mask", net_interface.netmask.u_addr.ip4 );
  print_ip( "GW", net_interface.gw.u_addr.ip4 );
  return 0;
}

rtems_shell_cmd_t shell_NETINFO_Command = {
  "netinfo",                                          /* name */
  "netinfo - shows basic network info, no arguments", /* usage */
  "net",                                              /* topic */
  shell_main_netinfo,                                 /* command */
  NULL,                                               /* alias */
  NULL                                                /* next */
};

static rtems_task Init( rtems_task_argument argument )
{
  rtems_status_code sc;
  int ret;

  TEST_BEGIN();

  ip_addr_t ipaddr, netmask, gw;

  IP_ADDR4( &ipaddr, 192, 168, 1, 2 );
  IP_ADDR4( &netmask, 255, 255, 255, 0 );
  IP_ADDR4( &gw, 192, 168, 1, 1 );
  unsigned char mac_ethernet_address[] = { 0x00, 0xd2, 0x79, 0x22, 0x22, 0x22 };

  ret = start_networking(
    &net_interface,
    &ipaddr,
    &netmask,
    &gw,
    mac_ethernet_address
  );

  if ( ret != 0 ) {
    return;
  }
  
  rtems_shell_init_environment();
  
  tcp_echoserver_init();

  sc = rtems_shell_init(
    "SHLL",                       /* task name */
    1024 * 4, /* task stack size */
    100,                          /* task priority */
    "/dev/console",               /* device name */
    false,                        /* run forever */
    true,                         /* wait for shell to terminate */
    NULL                          /* login check function,
                                     use NULL to disable a login check */
  );
  rtems_test_assert( sc == RTEMS_SUCCESSFUL );
  sys_arch_delay( 300000 );

  TEST_END();
  rtems_test_exit( 0 );
}

#define CONFIGURE_INIT

#define CONFIGURE_MICROSECONDS_PER_TICK 10000

#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS 32

#define CONFIGURE_SHELL_COMMANDS_INIT
#define CONFIGURE_SHELL_COMMANDS_ALL
#define CONFIGURE_SHELL_USER_COMMANDS &shell_NETINFO_Command

#define CONFIGURE_MAXIMUM_TASKS 6

#define CONFIGURE_MAXIMUM_POSIX_KEYS 1
#define CONFIGURE_MAXIMUM_SEMAPHORES 7
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES 3

#define CONFIGURE_INITIAL_EXTENSIONS RTEMS_TEST_INITIAL_EXTENSION

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_INIT_TASK_ATTRIBUTES RTEMS_FLOATING_POINT

//#define CONFIGURE_UNLIMITED_OBJECTS
#define CONFIGURE_UNIFIED_WORK_AREAS

#include <rtems/shellconfig.h>

#include <rtems/confdefs.h>
