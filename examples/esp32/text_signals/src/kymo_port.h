/* SPDX-License-Identifier: MIT */
/** @file kymo_port.h @brief JSON-lines diagnostic transport, no binary framing. */
#pragma once
#include <stdint.h>
/** @brief Send a JSON line. @param[in] id Channel. @param[in] value Sample. */
void textPortSend(uint32_t id, float value);
