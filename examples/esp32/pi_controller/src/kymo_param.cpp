/* SPDX-License-Identifier: MIT */
/** @file kymo_param.cpp @brief Writable controller parameters. */
#include "kymo_param.h"
#include "app_values.h"
static const KymoParameter params[] = {
    KYMO_PARAM(100, kp, 0.0f, 10.0f, ""),
    KYMO_PARAM(101, ki, 0.0f, 2.0f, ""),
    KYMO_PARAM(102, setpoint, 20.0f, 90.0f, "\xC2\xB0" "C"),
};

/*******************************************************************************
 * kymoParamConfig
 ******************************************************************************/
KymoEndpointParamConfig kymoParamConfig(void)
{
    KymoEndpointParamConfig config = {};
    config.table = params;
    config.count = KYMO_COUNT_OF(params);
    return config;
}

static KymoEndpointParam feature;
/*******************************************************************************
 * kymoParamInit
 ******************************************************************************/
KymoStatus kymoParamInit(KymoEndpoint *endpoint, const KymoEndpointParamConfig *config)
{
    KymoStatus status = KymoEndpointParam_Init(endpoint, &feature, config);
    return status;
}
