/**
@file              switch.h
@brief             The component api for switches and other digital inputs that require debouncing
@author            Peter Tjeerdsma
@version           $Id: switch.h 8183 2020-12-16 13:59:17Z fabio.seccamonte@philips.com $

@copyright         Copyright(C) 2018 Koninklijke Philips N.V., All Rights Reserved.
                   This source code and any compilation or derivative thereof is the
                   information of Koninklijke Philips N.V. is confidential in nature.
                   no circumstances is this software to be exposed to or placed
                   an Open Source License of any type without the expressed
                   permission of Koninklijke Philips N.V.

@note              This component will be extended and may use ifdef to create specific
                   configurations.
*/


#ifndef SWITCH_API_H_
#define SWITCH_API_H_

/*!
@ingroup brewsystem
@defgroup switch_driver_component Switch component
@{
Driver component for digital switches and other I/O
*/

/* ******************************************************************
                            INCLUDES
*********************************************************************/

/* include philips global file and all other headers */
//#include "philips_types.h"
//#include "product_config.h"  /* see note in header */
#include "fsl_common.h"


/* ******************************************************************
                            MACRO DEFINITIONS
 *********************************************************************/
typedef bool bool_t;
#define FALSE 0;
#define TRUE !FALSE;

/* ******************************************************************
                            EXPORTED TYPES
*********************************************************************/

/**
    @brief this enumerated type holds the generic switch edge state
*/
typedef enum
{
    SWITCH_NO_EDGE,                         /**< Switch no edge detected */
    SWITCH_RISING_EDGE,                     /**< Switch just made the transition from off */
    SWITCH_FALING_EDGE                      /**< Switch just made the transition from on */
} SWITCH_edge_t;


/**
    @brief this enumerated type holds the switch on/off state
*/
typedef enum
{
    SWITCH_OFF = 0,                         /**< Switch status is off, and/or faling edge */
    SWITCH_ON                               /**< Switch status is on,  and/or rising edge */
} SWITCH_state_t;


/* ******************************************************************
                    EXPORTED GLOBAL VARIABLES
*********************************************************************/

/* ******************************************************************
                    EXPORTED MODULE FUNCTIONS
*********************************************************************/

/** @brief The component initialization. Creates switch objects in RAM
*/
void SWITCH_init(void);

/** @brief  SWITCH_deinit stops the module
*/
void SWITCH_deinit(void);

/** @brief The component main handler. This task shall be called every 10ms
*/
void SWITCH_handler(void);

/** @brief test if switches are all initialized
*/
bool_t SWITCH_get_initialized(void);

/** @brief  Give the debounced switch status of the...
    @return SWITCH_state_t the current switch status (on, off)
*/
SWITCH_state_t get_Button1(void);
/** @brief  Give the debounced switch status of the...
    @return SWITCH_state_t the current switch status (on, off)
*/
SWITCH_edge_t get_Edge_Button1(void);

/** @brief  Give the debounced switch status of the...
    @return SWITCH_state_t the current switch status (on, off)
*/
SWITCH_state_t get_Button2(void);


/**@}*/
#endif /* SWITCH_API_H_ */
