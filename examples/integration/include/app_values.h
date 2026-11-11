/* SPDX-License-Identifier: MIT */
/** @file app_values.h
 * @brief Application values shared by telemetry bindings, without feature APIs.
 * @details Single superloop owner; add synchronization for concurrent producers.
 * @par Usage Plot reads, Param modifies and Watch observes the same gain.
 */
#ifndef EXAMPLE_APP_VALUES_H
#define EXAMPLE_APP_VALUES_H
/** @brief Dimensionless gain, initially 1; Parameter permits 0..10. */
extern float appGain;
#endif
