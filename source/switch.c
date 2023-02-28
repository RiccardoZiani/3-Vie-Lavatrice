/**
@file              switch.c
@brief             Component that reads and debounces the switches and other I/O.
@author            PETJ
@version           $Id: switch.c 10427 2022-04-19 16:50:49Z massimo.mazzoni@philips.com $

@copyright         Copyright(C) 2018 Koninklijke Philips N.V., All Rights Reserved.
                   This source code and any compilation or derivative thereof is the
                   information of Koninklijke Philips N.V. is confidential in nature.
                   no circumstances is this software to be exposed to or placed
                   an Open Source License of any type without the expressed
                   permission of Koninklijke Philips N.V.
*/

/* ******************************************************************
                            INCLUDES
*********************************************************************/

#include "switch.h"
#include "pin_mux.h"
#include "fsl_gpio.h"


/* ******************************************************************
                            MACRO DEFINITIONS
*********************************************************************/
/* Note: The debounce maximum is 255 (times 10ms) */
#define DEBOUNCE_NORMAL (50)
#define DEBOUNCE_FAST   (20)

/* ******************************************************************
                         NON EXPORTED LOCAL TYPES
*********************************************************************/

typedef struct
{
    uint32_t debounce_cntr      : 8;            /* debounce counter             */
    uint32_t counts_needed      : 8;            /* Number of counts the io state must be stable  */
    bool_t   stable_io_state    : 1;            /* stable io value              */
    bool_t   new_io_state       : 1;            /* updated/new value            */
    uint32_t initialized        : 1;            /* Initial status determined    */
    SWITCH_state_t switch_state : 2;            /* switch state (typed)         */
    SWITCH_edge_t  switch_edge  : 3;            /* switch edge state (typed)    */
} DEBOUNCE_OBJECT_TYPE; /* Bit fields struct */


enum SWITCHES_TAG
{
    Button1 = 0,
    Button2,
    Number_of_Buttons_OBJ
};

/* ******************************************************************
                     GLOBAL VARIABLES
*********************************************************************/

/* ******************************************************************
                     LOCAL VARIABLES
*********************************************************************/
static DEBOUNCE_OBJECT_TYPE brew_system_switches[Number_of_Buttons_OBJ]; /* Static structure of debounce objects */


/* ******************************************************************
                LOCAL FUNCTIONS DECLARATIONS
*********************************************************************/

/** @brief  Initializes a debounce object instance.
    @param  initObject
    @param  debounce_counts_needed
    @param  first_read_value
    @return none.
*/
static void debounce_initialize_instance ( DEBOUNCE_OBJECT_TYPE* initObject, uint8_t debounce_counts_needed, bool_t first_read_value );

/** @brief  Proces one count/cycle of the debounce object instance.
    @param  handleObject
    @return none
*/
static void debounce_handle_instance ( DEBOUNCE_OBJECT_TYPE* handleObject );


/* ******************************************************************
                    ALL EXPORTED FUNCTIONS IMPLEMENTATIONS
*********************************************************************/
void SWITCH_init(void)
{
    debounce_initialize_instance(&brew_system_switches[Button1], DEBOUNCE_NORMAL, GPIO_PinRead(BOARD_INITPINS_Button1_GPIO_PORT, BOARD_INITPINS_Button1_PIN));
    debounce_initialize_instance(&brew_system_switches[Button2], DEBOUNCE_FAST, GPIO_PinRead(BOARD_INITPINS_Button2_GPIO_PORT, BOARD_INITPINS_Button2_PIN));
    return;
}


void SWITCH_handler(void)
{
    brew_system_switches[Button1].new_io_state = GPIO_PinRead(BOARD_INITPINS_Button1_GPIO_PORT, BOARD_INITPINS_Button1_PIN);
    debounce_handle_instance(&brew_system_switches[Button1]);

    brew_system_switches[Button2].new_io_state = GPIO_PinRead(BOARD_INITPINS_Button2_GPIO_PORT, BOARD_INITPINS_Button2_PIN);
    debounce_handle_instance(&brew_system_switches[Button2]);


    return;
}


SWITCH_state_t get_Button1(void)  /* SWITCH_OFF means BU door is open */
{
    return brew_system_switches[Button1].switch_state;
}

SWITCH_edge_t get_Edge_Button1(void)
{
    return brew_system_switches[Button1].switch_edge;
}

SWITCH_state_t get_Button2(void)  /* SWITCH_OFF means drip tray is not in */
{
    return brew_system_switches[Button2].switch_state;
}


/* ******************************************************************
                    ALL LOCAL FUNCTIONS IMPLEMENTATIONS
*********************************************************************/

static void debounce_initialize_instance ( DEBOUNCE_OBJECT_TYPE* initObject, uint8_t debounce_counts_needed, bool_t first_read_value )
{
    initObject->debounce_cntr    = debounce_counts_needed;
    initObject->counts_needed    = debounce_counts_needed;
    initObject->stable_io_state  = first_read_value;
    initObject->new_io_state     = first_read_value;
    initObject->initialized      = TRUE;
    if (first_read_value)
    {
        initObject->switch_state = SWITCH_ON;
    }
    else
    {
        initObject->switch_state = SWITCH_OFF;
    }
    initObject->switch_edge      = SWITCH_NO_EDGE;
    return;
}


static void debounce_handle_instance ( DEBOUNCE_OBJECT_TYPE* handleObject )
{
    /* Check if the new sample is still equal to the stable value. When this is
     * the case, the stable value stays the same and no event has to be
     * generate. Leave the debounce counter on the max value
     */
    if (handleObject->new_io_state == handleObject->stable_io_state)
    {
        handleObject->debounce_cntr = handleObject->counts_needed;
        handleObject->initialized = TRUE;
        /* Reset the edge triggers */
        handleObject->switch_edge = SWITCH_NO_EDGE;
    }
    else
    {
        if (0 == handleObject->debounce_cntr)
        {
            /* The stable io value and the new sample do not match and the debounce counter
            * has expired. This means that the new value can be seen as stable */
            handleObject->stable_io_state = handleObject->new_io_state;
            /* Reset the doubounce counter */
            handleObject->debounce_cntr = handleObject->counts_needed;
            handleObject->initialized = TRUE;

            /* Check on positive or negative edge. */
            if ( handleObject->stable_io_state )
            {
                handleObject->switch_state = SWITCH_ON;
                handleObject->switch_edge  = SWITCH_RISING_EDGE;
            }
            else
            {
                handleObject->switch_state = SWITCH_OFF;
                handleObject->switch_edge  = SWITCH_FALING_EDGE;
            }
        }
        else
        {
            /* The stable value and the new sample do not match but the debounce counter
            * has not yet expired. This means that the new value is not yet stable
            * and debouncing continues
            */
            handleObject->debounce_cntr--;
        }
    }
    return;
}

