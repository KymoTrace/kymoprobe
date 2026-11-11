/* SPDX-License-Identifier: MIT */
/** @file kymo_internal.h
 * @brief Private connection between project base and feature files.
 * @par Usage Application code includes kymo_init.h or its feature header instead.
 */
#ifndef EXAMPLE_KYMO_INTERNAL_H
#define EXAMPLE_KYMO_INTERNAL_H
#include <endpoint/kymo_endpoint.h>
/** @brief Access the persistent project Endpoint for feature registration.
 * @return Base context; never reset or copy it.
 * @par Usage Internal feature Init calls only. */
KymoEndpoint *kymoEndpoint(void);
#endif
