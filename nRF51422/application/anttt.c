/**********************************************************************************************************************
File: anttt.c                                                                

Description:
Implements TIC-TAC-TOE using data input from ANT, BLE, or SPI.

**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_xxAnttt"
***********************************************************************************************************************/
/* New variables */
u32 G_u32AntttFlags;                                     /* Global state flags */

/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */
extern volatile u32 G_u32BPEngenuicsFlags;             /* From bleperipheral_engenuics.c  */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "Anttt_<type>" and be declared as static.
***********************************************************************************************************************/
static fnCode_type Anttt_pfnStateMachine;              /* The application state machine function pointer */

static u8 Anttt_au8SpiReceiveBuffer[U8_SPI0_BUFFER_SIZE];


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: AntttInitialize

Description:
Initializes the State Machine and its variables.

Requires:

Promises:
*/
void AntttInitialize(void)
{
  u8 au8SpiTestMessage[] = {NRF_SYNC_BYTE, NRF_CMD_TEST_LENGTH, NRF_CMD_TEST};
  u8 u8ReceivedBytes;
  
  /* Send a test message via SPI to check connection to SAM3U2 */
  LedOn(RED);
  SpiMasterSend(au8SpiTestMessage, sizeof(au8SpiTestMessage) );
  
  /* Wait for response */
  for(u32 i = 0; i < 10000; i++);
  if(NRF_GPIOTE->EVENTS_IN[EVENT_MRDY_ASSERTED])
  {
    /* Clear the MRDY event and receive bytes */
    NRF_GPIOTE->EVENTS_IN[EVENT_MRDY_ASSERTED] = 0;
    u8ReceivedBytes = SpiMasterReceive(Anttt_au8SpiReceiveBuffer);
    
    /* Check if the received message is the expected test response */
    if( (Anttt_au8SpiReceiveBuffer[NRF_SYNC_INDEX] == NRF_SYNC_BYTE) &&
        (Anttt_au8SpiReceiveBuffer[NRF_COMMAND_INDEX] == NRF_CMD_TEST_RESPONSE))
    {
      LedOff(RED);
    }
  }

  /* Activate blinking blue LED to indicate BLE is searching */
  LedBlink(BLUE, LED_4HZ);  
  
  Anttt_pfnStateMachine = AntttSM_Idle;
  
} /* end AntttInitialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function AntttRunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void AntttRunActiveState(void)
{
  Anttt_pfnStateMachine();

} /* end AntttRunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------------------------------------------------*/
/* State Machine definitions                                                                                          */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
State: AntttSM_Idle
The program looks for a connection to a game, either from BLE, ANT,
or over SPI. This first command must be a GAME_REQUEST.
*/
static void AntttSM_Idle(void)
{
  // Check if module is connected to client.
  if (G_u32BPEngenuicsFlags == _BPENGENUICS_CONNECTED)
  {
    // Set LEDs and proceed to wait state.
    LedOn(BLUE);   // Connected to Client.
       
  }
 
} 



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
