/*
 * The olsr.org Optimized Link-State Routing daemon(olsrd)
 * Copyright (c) 2004, Andreas Tonnesen(andreto@olsr.org)
 *                     includes code by Bruno Randolf
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 * * Neither the name of olsr.org, olsrd nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Visit http://www.olsr.org for more information.
 *
 * If you find this software useful feel free to make a donation
 * to the project. For more information see the website or contact
 * the copyright holders.
 *
 */

#ifndef _OLSRD_JSONINFO_PRINTERS_H
#define _OLSRD_JSONINFO_PRINTERS_H

#include <stdbool.h>
#include <time.h>

#include "common/autobuf.h"

extern struct timeval start_time;

void plugin_init(const char * plugin_name);

const char * determine_mime_type(unsigned int send_what);

void ipc_print_neighbors(struct autobuf *abuf, bool list_2hop);
void ipc_print_links(struct autobuf *abuf);
void ipc_print_routes(struct autobuf *abuf);
void ipc_print_topology(struct autobuf *abuf);
void ipc_print_hna(struct autobuf *abuf);
void ipc_print_mid(struct autobuf *abuf);
void ipc_print_gateways(struct autobuf *abuf);
void ipc_print_sgw(struct autobuf *abuf);
void ipc_print_version(struct autobuf *abuf);
void ipc_print_olsrd_conf(struct autobuf *abuf);
void ipc_print_interfaces(struct autobuf *abuf);
void ipc_print_config(struct autobuf *abuf);
void ipc_print_plugins(struct autobuf *abuf);

#endif /* _OLSRD_JSONINFO_PRINTERS_H */