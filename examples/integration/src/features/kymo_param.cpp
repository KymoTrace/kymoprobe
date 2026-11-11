/* SPDX-License-Identifier: MIT */
/** @file kymo_param.cpp
 * @brief Param-specific bindings and complete initialization.
 */
#include "features/kymo_param.h"
#if KYMO_EFFECTIVE_PARAMETERS
#include "../detail/kymo_internal.h"
#include "app_values.h"
static KymoEndpointParam sFeature;
static const uint8_t GAIN_PARAMETER_ID = 100u;
static const float GAIN_MINIMUM = 0.0f;
static const float GAIN_MAXIMUM = 10.0f;
static const KymoParameter PARAMETERS[] = {
    { GAIN_PARAMETER_ID, &appGain, GAIN_MINIMUM, GAIN_MAXIMUM, "gain", "" }
};
/*******************************************************************************
 * kymoParamConfig
 ******************************************************************************/
KymoEndpointParamConfig kymoParamConfig(void)
{
    KymoEndpointParamConfig config = {};
    config.table = PARAMETERS;
    config.count = KYMO_COUNT_OF(PARAMETERS);
    return config;
}
/*******************************************************************************
 * KymoApp_ParamInit
 ******************************************************************************/
KymoStatus KymoApp_ParamInit(const KymoEndpointParamConfig *config)
{
    KymoStatus status = KymoEndpointParam_Init(kymoEndpoint(), &sFeature, config);
    return status;
}

#endif /* enabled feature */
