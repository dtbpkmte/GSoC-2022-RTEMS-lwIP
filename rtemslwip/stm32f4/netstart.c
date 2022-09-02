/*
 * Copyright (C) 2022 Duc Doan (dtbpkmte at gmail.com)
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

#include <netstart.h>
#include <lwip/tcpip.h>
#include "lwip/init.h"
#include "lwip/netif.h"
#include "ethernetif.h"

int start_networking(
  struct netif  *net_interface,
  ip_addr_t     *ipaddr,
  ip_addr_t     *netmask,
  ip_addr_t     *gateway,
  unsigned char *mac_ethernet_address
)
{
  tcpip_init( NULL, NULL );
  
  set_mac_addr(mac_ethernet_address);

  netif_add(
    net_interface, 
    &ipaddr->u_addr.ip4, 
    &netmask->u_addr.ip4, 
    &gateway->u_addr.ip4, 
    NULL, 
    ethernetif_init, 
    tcpip_input
  );
  
  netif_set_default(net_interface);
  
  if (netif_is_link_up(net_interface)) {
    netif_set_up(net_interface);
    
    sys_thread_new(
      "stm32f4_ethernet_link_thread",
      ethernet_link_thread,
      net_interface,
      1024,
      DEFAULT_THREAD_PRIO
    );
        
    return 0;
  } else {
    netif_set_down(net_interface);
  }

  return -1;
}
