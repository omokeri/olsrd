/*
 * The olsr.org Optimized Link-State Routing daemon(olsrd)
 * Copyright (c) 2004
 *
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

#include "json_helpers.h"
#include "olsr.h"
#include "ipcalc.h"

#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>
#include <float.h>

#ifdef __linux__
#include <fcntl.h>
#endif /* __linux__ */

static const char * empty = "";

/* JSON support functions */

/* JSON does not allow commas dangling at the end of arrays, so we need to
 * count which entry number we're at in order to make sure we don't tack a
 * dangling comma on at the end */
#define ENTRY_NUMBER_MAX_DEPTH 16
static int entrynumber[ENTRY_NUMBER_MAX_DEPTH] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static int currentjsondepth = 0;

void abuf_json_reset_entry_number_and_depth(void) {
  entrynumber[0] = 0;
  currentjsondepth = 0;
}

static void abuf_json_new_indent(struct autobuf *abuf) {
  assert(abuf);

  if (currentjsondepth) {
    int i = currentjsondepth;

    abuf_puts(abuf, "\n");
    while (i-- > 0) {
      abuf_puts(abuf, "  ");
    }
  }
}

void abuf_json_insert_comma(struct autobuf *abuf) {
  assert(abuf);

  if (entrynumber[currentjsondepth])
    abuf_appendf(abuf, ",");
}

void abuf_json_mark_output(bool open, struct autobuf *abuf) {
  assert(abuf);

  if (open) {
    assert(!currentjsondepth);
    abuf_json_new_indent(abuf);
    abuf_puts(abuf, "{");
    currentjsondepth++;
    entrynumber[currentjsondepth] = 0;
  } else {
    assert(currentjsondepth == 1);
    entrynumber[currentjsondepth] = 0;
    currentjsondepth--;
    abuf_json_new_indent(abuf);
    abuf_puts(abuf, "\n}");
  }
}

void abuf_json_mark_object(bool open, bool array, struct autobuf *abuf, const char* header) {
  assert(abuf);

  if (open) {
    abuf_json_insert_comma(abuf);
    abuf_json_new_indent(abuf);
    if (header) {
      abuf_appendf(abuf, "\"%s\": %s", header, array ? "[" : "{");
    } else {
      abuf_appendf(abuf, "%s", array ? "[" : "{");
    }
    entrynumber[currentjsondepth]++;
    currentjsondepth++;
    assert(currentjsondepth < ENTRY_NUMBER_MAX_DEPTH);
    entrynumber[currentjsondepth] = 0;
  } else {
    entrynumber[currentjsondepth] = 0;
    currentjsondepth--;
    assert(currentjsondepth >= 0);
    abuf_json_new_indent(abuf);
    abuf_appendf(abuf, "%s", array ? "]" : "}");
  }
}

void abuf_json_mark_array_entry(bool open, struct autobuf *abuf) {
  assert(abuf);

  abuf_json_mark_object(open, false, abuf, NULL);
}

void abuf_json_boolean(struct autobuf *abuf, const char* key, bool value) {
  assert(abuf);
  assert(key);

  abuf_json_insert_comma(abuf);
  abuf_json_new_indent(abuf);
  abuf_appendf(abuf, "\"%s\": %s", key, value ? "true" : "false");
  entrynumber[currentjsondepth]++;
}

void abuf_json_string(struct autobuf *abuf, const char* key, const char* value) {
  const char * val;

  assert(abuf);
  assert(key || value);

  if (!value) {
    val = empty;
  } else {
    val = value;
  }

  abuf_json_insert_comma(abuf);
  abuf_json_new_indent(abuf);
  if (!key) {
    abuf_appendf(abuf, "\"%s\"", value);
  } else {
    abuf_appendf(abuf, "\"%s\": \"%s\"", key, val);
  }
  entrynumber[currentjsondepth]++;
}

void abuf_json_int(struct autobuf *abuf, const char* key, long long value) {
  const char * fmt;

  assert(abuf);
  assert(key);

#ifndef _WIN32
  fmt = "\"%s\": %lld";
#else
  fmt = "\"%s\": %ld";
#endif

  abuf_json_insert_comma(abuf);
  abuf_json_new_indent(abuf);
  abuf_appendf(abuf, fmt, key, value);
  entrynumber[currentjsondepth]++;
}

void abuf_json_float(struct autobuf *abuf, const char* key, double value) {
  double v = value;
  int isInf = isinf(v);

  assert(abuf);
  assert(key);

  if (isnan(v)) {
    v = 0.0;
  } else if (isInf < 0) {
    v = -DBL_MAX;
  } else if (isInf > 0) {
    v = DBL_MAX;
  }

  abuf_json_insert_comma(abuf);
  abuf_json_new_indent(abuf);
  abuf_appendf(abuf, "\"%s\": %f", key, v);
  entrynumber[currentjsondepth]++;
}

void abuf_json_ip_address(struct autobuf *abuf, const char* key, union olsr_ip_addr *ip) {
  struct ipaddr_str ipStr;
  const char * value;

  assert(abuf);
  assert(key || ip);

  if (!ip) {
    value = empty;
  } else {
    value = olsr_ip_to_string(&ipStr, ip);
  }

  abuf_json_insert_comma(abuf);
  abuf_json_new_indent(abuf);
  if (!key) {
    abuf_appendf(abuf, "\"%s\"", value);
  } else {
    abuf_appendf(abuf, "\"%s\": \"%s\"", key, value);
  }
  entrynumber[currentjsondepth]++;
}

void abuf_json_ip_address46(struct autobuf *abuf, const char* key, void *ip, int af) {
  struct ipaddr_str ipStr;
  const char * value;

  assert(abuf);
  assert(key || ip);

  if (!ip) {
    value = empty;
  } else if (af == AF_INET) {
    value = ip4_to_string(&ipStr, *((const struct in_addr*) ip));
  } else {
    value = ip6_to_string(&ipStr, (const struct in6_addr * const ) ip);
  }

  abuf_json_insert_comma(abuf);
  abuf_json_new_indent(abuf);
  if (!key) {
    abuf_appendf(abuf, "\"%s\"", value);
  } else {
    abuf_appendf(abuf, "\"%s\": \"%s\"", key, value);
  }
  entrynumber[currentjsondepth]++;
}

void abuf_json_prefix(struct autobuf *abuf, const char* key, struct olsr_ip_prefix *prefix) {
  struct ipaddr_str ipStr;
  const char * value;

  assert(abuf);
  assert(key || prefix);

  if (!prefix) {
    value = empty;
  } else {
    value = olsr_ip_to_string(&ipStr, &prefix->prefix);
  }

  abuf_json_insert_comma(abuf);
  abuf_json_new_indent(abuf);
  if (!key) {
    abuf_appendf(abuf, "\"%s/%d\"", value, prefix->prefix_len);
  } else {
    abuf_appendf(abuf, "\"%s\": \"%s/%d\"", key, value, prefix->prefix_len);
  }
  entrynumber[currentjsondepth]++;
}