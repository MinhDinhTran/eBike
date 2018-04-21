/*********************************************************************
 * INCLUDES
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "bcomdef.h"
#include "OSAL.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"
#include "OnBoard.h"
#include "simpleGATTprofile.h"
        
#include "hal_adc.h"
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

#define SERVAPP_NUM_ATTR_SUPPORTED      27

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
// Simple GATT Profile Service UUID: 0xFFF0
CONST uint8 simpleProfileServUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_SERV_UUID), HI_UINT16(SIMPLEPROFILE_SERV_UUID)
};

#define createUUID(name) CONST uint8 _ ## name[] = { LO_UINT16(name), HI_UINT16(name) }
createUUID(UUID_MODE_ID);
createUUID(UUID_PWM_DUTY_CYCLE_ID);
createUUID(UUID_V_THRESHOLD_ID);
createUUID(UUID_BIKE_BATTERY_LEVEL_ID);
createUUID(UUID_CURRENT_ID);
createUUID(UUID_BIKE_SPEED_ID);
createUUID(UUID_BIKE_FLAGS_ID);

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

static simpleProfileCBs_t *simpleProfile_AppCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

// Simple Profile Service attribute 
// GATT_PROP_READ | GATT_PROP_WRITE | GATT_PROP_NOTIFY
static CONST gattAttrType_t simpleProfileService = { ATT_BT_UUID_SIZE, simpleProfileServUUID };

static uint8 _MODE_Props = GATT_PROP_READ|GATT_PROP_WRITE;
uint8 _MODE_Value = 0;
static uint8 _MODE_Desc[] = "Mode";

static uint8 _PWM_DUTY_CYCLE_Props = GATT_PROP_READ|GATT_PROP_WRITE;
uint8 _PWM_DUTY_CYCLE_Value = 0;
static uint8 _PWM_DUTY_CYCLE_Desc[] = "PWM";

static uint8 _V_THRESHOLD_Props = GATT_PROP_READ|GATT_PROP_WRITE;
uint32 _V_THRESHOLD_Value = 0;
static uint8 _V_THRESHOLD_Desc[] = "V Threshold";

static uint8 _BIKE_BATTERY_LEVEL_Props = GATT_PROP_READ|GATT_PROP_NOTIFY;
static uint16 _BIKE_BATTERY_LEVEL_Value = 0;
static uint8 _BIKE_BATTERY_LEVEL_Desc[] = "U_BAT";
static gattCharCfg_t* _BIKE_BATTERY_LEVEL_NotiConfig;

static uint8 _CURRENT_Props = GATT_PROP_READ|GATT_PROP_NOTIFY;
static uint16 _CURRENT_Value = 0;
static uint8 _CURRENT_Desc[] = "I";
static gattCharCfg_t* _CURRENT_NotiConfig;

static uint8 _BIKE_SPEED_Props = GATT_PROP_READ|GATT_PROP_NOTIFY;
static float _BIKE_SPEED_Value = 0;
static uint8 _BIKE_SPEED_Desc[] = "Speed";
static gattCharCfg_t* _BIKE_SPEED_NotiConfig;

static uint8 _BIKE_FLAGS_Props = GATT_PROP_READ|GATT_PROP_NOTIFY;
static uint32 _BIKE_FLAGS_Value = 0;
static uint8 _BIKE_FLAGS_Desc[] = ".";
static gattCharCfg_t* _BIKE_FLAGS_NotiConfig;
/*********************************************************************
 * Profile Attributes - Table
 */
#define ATT_Declaration(permission, props) {{ ATT_BT_UUID_SIZE, characterUUID }, permission, 0, props},
#define ATT_Value_Declaration(permission, props, UUID) {{ ATT_BT_UUID_SIZE, UUID }, permission, 0, (uint8*)props},
#define ATT_Desc_Declaration(permission, props) {{ ATT_BT_UUID_SIZE, charUserDescUUID }, permission, 0, props},
#define ATT_Noti_Declaration(props) {{ ATT_BT_UUID_SIZE, clientCharCfgUUID }, GATT_PERMIT_READ | GATT_PERMIT_WRITE, 0, (uint8 *)props},
static gattAttribute_t simpleProfileAttrTbl[SERVAPP_NUM_ATTR_SUPPORTED] = 
{
  // Simple Profile Service
  { 
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8 *)&simpleProfileService            /* pValue */
  },
  
     //   GATT_PERMIT_WRITE|GATT_PERMIT_READ, GATT_PERMIT_AUTHEN_READ | GATT_PERMIT_AUTHEN_WRITE, 
   ATT_Declaration(GATT_PERMIT_READ|GATT_PERMIT_WRITE,          &_MODE_Props)
   ATT_Value_Declaration(GATT_PERMIT_READ|GATT_PERMIT_WRITE,    &_MODE_Value, _UUID_MODE_ID )
   ATT_Desc_Declaration(GATT_PERMIT_READ|GATT_PERMIT_WRITE,     _MODE_Desc)

   ATT_Declaration(GATT_PERMIT_READ|GATT_PERMIT_WRITE,          &_PWM_DUTY_CYCLE_Props)
   ATT_Value_Declaration(GATT_PERMIT_READ|GATT_PERMIT_WRITE,    &_PWM_DUTY_CYCLE_Value, _UUID_PWM_DUTY_CYCLE_ID )
   ATT_Desc_Declaration(GATT_PERMIT_READ|GATT_PERMIT_WRITE,     _PWM_DUTY_CYCLE_Desc)

   ATT_Declaration(GATT_PERMIT_READ|GATT_PERMIT_WRITE,          &_V_THRESHOLD_Props)
   ATT_Value_Declaration(GATT_PERMIT_READ|GATT_PERMIT_WRITE,    &_V_THRESHOLD_Value, _UUID_V_THRESHOLD_ID )
   ATT_Desc_Declaration(GATT_PERMIT_READ|GATT_PERMIT_WRITE,     _V_THRESHOLD_Desc)


   ATT_Declaration(GATT_PERMIT_READ,          &_BIKE_BATTERY_LEVEL_Props)
   ATT_Value_Declaration(GATT_PERMIT_READ,    &_BIKE_BATTERY_LEVEL_Value, _UUID_BIKE_BATTERY_LEVEL_ID )
   ATT_Desc_Declaration(GATT_PERMIT_READ,     _BIKE_BATTERY_LEVEL_Desc)
   ATT_Noti_Declaration(                      &_BIKE_BATTERY_LEVEL_NotiConfig)        // Characteristic configuration

   ATT_Declaration(GATT_PERMIT_READ,          &_CURRENT_Props)
   ATT_Value_Declaration(GATT_PERMIT_READ,    &_CURRENT_Value, _UUID_CURRENT_ID )
   ATT_Desc_Declaration(GATT_PERMIT_READ,     _CURRENT_Desc)
   ATT_Noti_Declaration(                      &_CURRENT_NotiConfig)  
     
   ATT_Declaration(GATT_PERMIT_READ,          &_BIKE_SPEED_Props)
   ATT_Value_Declaration(GATT_PERMIT_READ,    &_BIKE_SPEED_Value, _UUID_BIKE_SPEED_ID )
   ATT_Desc_Declaration(GATT_PERMIT_READ,     _BIKE_SPEED_Desc)
   ATT_Noti_Declaration(                      &_BIKE_SPEED_NotiConfig)  
     
   ATT_Declaration(GATT_PERMIT_READ,          &_BIKE_FLAGS_Props)
   ATT_Value_Declaration(GATT_PERMIT_READ,    &_BIKE_FLAGS_Value, _UUID_BIKE_FLAGS_ID )
   ATT_Desc_Declaration(GATT_PERMIT_READ,     _BIKE_FLAGS_Desc)
   ATT_Noti_Declaration(                      &_BIKE_FLAGS_NotiConfig)  
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t simpleProfile_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr, 
                                           uint8 *pValue, uint8 *pLen, uint16 offset,
                                           uint8 maxLen, uint8 method );
static bStatus_t simpleProfile_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                            uint8 *pValue, uint8 len, uint16 offset,
                                            uint8 method );

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Simple Profile Service Callbacks
CONST gattServiceCBs_t simpleProfileCBs =
{
  simpleProfile_ReadAttrCB,  // Read callback function pointer
  simpleProfile_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SimpleProfile_AddService
 *
 * @brief   Initializes the Simple Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
bStatus_t SimpleProfile_AddService( uint32 services )
{
  uint8 status;
  
  // Allocate Client Characteristic Configuration table
  _BIKE_BATTERY_LEVEL_NotiConfig = (gattCharCfg_t *)osal_mem_alloc( sizeof(gattCharCfg_t) *
                                                              linkDBNumConns );
  _CURRENT_NotiConfig = (gattCharCfg_t *)osal_mem_alloc( sizeof(gattCharCfg_t) *
                                                              linkDBNumConns );
  
  _BIKE_SPEED_NotiConfig = (gattCharCfg_t *)osal_mem_alloc( sizeof(gattCharCfg_t) *
                                                              linkDBNumConns );
  
  _BIKE_FLAGS_NotiConfig = (gattCharCfg_t *)osal_mem_alloc( sizeof(gattCharCfg_t) *
                                                              linkDBNumConns );
  
  if ( _BIKE_BATTERY_LEVEL_NotiConfig == NULL || _CURRENT_NotiConfig == NULL 
      || _BIKE_SPEED_NotiConfig == NULL || _BIKE_FLAGS_NotiConfig == NULL)
  {     
    return ( bleMemAllocError );
  }
  
 
  
  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, _BIKE_BATTERY_LEVEL_NotiConfig );
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, _CURRENT_NotiConfig );
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, _BIKE_SPEED_NotiConfig );
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, _BIKE_FLAGS_NotiConfig );
  
  if ( services & SIMPLEPROFILE_SERVICE )
  {
    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService( simpleProfileAttrTbl, 
                                          GATT_NUM_ATTRS( simpleProfileAttrTbl ),
                                          GATT_MAX_ENCRYPT_KEY_SIZE,
                                          &simpleProfileCBs );
  }
  else
  {
    status = SUCCESS;
  }
  
  return ( status );
}

/*********************************************************************
 * @fn      SimpleProfile_RegisterAppCBs
 *
 * @brief   Registers the application callback function. Only call 
 *          this function once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
bStatus_t SimpleProfile_RegisterAppCBs( simpleProfileCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    simpleProfile_AppCBs = appCallbacks;
    
    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}

/*********************************************************************
 * @fn      SimpleProfile_SetParameter
 *
 * @brief   Set a Simple Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to write
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
#define SetValue(type, var) if ( len == sizeof (type) ) var = *((type*)value); else ret = bleInvalidRange;
bStatus_t SimpleProfile_SetParameter( uint8 param, uint8 len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case MODE_ID:
      SetValue(uint8, _MODE_Value)
      break;
      
    case PWM_DUTY_CYCLE_ID:
      SetValue(uint8, _PWM_DUTY_CYCLE_Value)
      break;
      
    case V_THRESHOLD_ID:
      SetValue(uint32, _V_THRESHOLD_Value)
      break;
      
    case BIKE_BATTERY_LEVEL_ID:
      SetValue(uint16, _BIKE_BATTERY_LEVEL_Value);      
     if ( GATTServApp_ProcessCharCfg( _BIKE_BATTERY_LEVEL_NotiConfig, ((uint8*)&_BIKE_BATTERY_LEVEL_Value), FALSE,
                                  simpleProfileAttrTbl, GATT_NUM_ATTRS( simpleProfileAttrTbl ),
                                  INVALID_TASK_ID, simpleProfile_ReadAttrCB ) != SUCCESS)
     {
       printf("WTF\n");
       SystemReset();
     }
      break;
    case CURRENT_ID:
      SetValue(uint16, _CURRENT_Value);      
     if ( GATTServApp_ProcessCharCfg( _CURRENT_NotiConfig, ((uint8*)&_CURRENT_Value), FALSE,
                                  simpleProfileAttrTbl, GATT_NUM_ATTRS( simpleProfileAttrTbl ),
                                  INVALID_TASK_ID, simpleProfile_ReadAttrCB ) != SUCCESS)
     {
       printf("WTF\n");
       SystemReset();
     }
      break;
    case BIKE_SPEED_ID:
      SetValue(float, _BIKE_SPEED_Value);
      
     if ( GATTServApp_ProcessCharCfg( _BIKE_SPEED_NotiConfig, ((uint8*)&_BIKE_SPEED_Value), FALSE,
                                  simpleProfileAttrTbl, GATT_NUM_ATTRS( simpleProfileAttrTbl ),
                                  INVALID_TASK_ID, simpleProfile_ReadAttrCB ) != SUCCESS)
     {
       printf("WTF\n");
       SystemReset();
     }
      break;
    case BIKE_FLAGS_ID:
      SetValue(uint32, _BIKE_FLAGS_Value);      
     if ( GATTServApp_ProcessCharCfg( _BIKE_FLAGS_NotiConfig, ((uint8*)&_BIKE_FLAGS_Value), FALSE,
                                  simpleProfileAttrTbl, GATT_NUM_ATTRS( simpleProfileAttrTbl ),
                                  INVALID_TASK_ID, simpleProfile_ReadAttrCB ) != SUCCESS)
     {
       printf("WTF\n");
       SystemReset();
     }
      break;
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return ( ret );
}

/*********************************************************************
 * @fn      SimpleProfile_GetParameter
 *
 * @brief   Get a Simple Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t SimpleProfile_GetParameter( uint8 param, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case MODE_ID:
      VOID memcpy( value, &_MODE_Value, MODE_LEN );
      break;
      
    case PWM_DUTY_CYCLE_ID:
      VOID memcpy( value, &_PWM_DUTY_CYCLE_Value, PWM_DUTY_CYCLE_LEN );
      break;
      
    case V_THRESHOLD_ID:
      VOID memcpy( value, &_V_THRESHOLD_Value, V_THRESHOLD_LEN );
      break;
      
    case BIKE_BATTERY_LEVEL_ID:
      VOID memcpy( value, &_BIKE_BATTERY_LEVEL_Value, BIKE_BATTERY_LEVEL_LEN );
      break;
    case CURRENT_ID:
      VOID memcpy( value, &_CURRENT_Value, CURRENT_LEN );
      break;
    case BIKE_SPEED_ID:
      VOID memcpy( value, &_BIKE_SPEED_Value, BIKE_SPEED_LEN );
      break;
    case BIKE_FLAGS_ID:
      VOID memcpy( value, &_BIKE_FLAGS_Value, BIKE_FLAGS_LEN );
      break;
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return ( ret );
}

/*********************************************************************
 * @fn          simpleProfile_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      SUCCESS, blePending or Failure
 */
static bStatus_t simpleProfile_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr, 
                                           uint8 *pValue, uint8 *pLen, uint16 offset,
                                           uint8 maxLen, uint8 method )
{   
  bStatus_t status = SUCCESS;

  // If attribute permissions require authorization to read, return error
  if ( gattPermitAuthorRead( pAttr->permissions ) )
  {
    // Insufficient authorization
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );
  }
  
  // Make sure it's not a blob operation (no attributes in the profile are long)
  if ( offset > 0 )
  {
    return ( ATT_ERR_ATTR_NOT_LONG );
  }
 
  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {
    // 16-bit UUID
    uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch ( uuid )
    {
      case UUID_MODE_ID:
        *pLen = MODE_LEN;
        VOID memcpy( pValue, pAttr->pValue, MODE_LEN );
        break;
      case UUID_PWM_DUTY_CYCLE_ID:
        *pLen = PWM_DUTY_CYCLE_LEN;
        VOID memcpy( pValue, pAttr->pValue, PWM_DUTY_CYCLE_LEN );
        break;
      case UUID_V_THRESHOLD_ID:
        *pLen = V_THRESHOLD_LEN;
        VOID memcpy( pValue, pAttr->pValue, V_THRESHOLD_LEN );
        break;
      case UUID_BIKE_BATTERY_LEVEL_ID:
        *pLen = BIKE_BATTERY_LEVEL_LEN;
        VOID memcpy( pValue, pAttr->pValue, BIKE_BATTERY_LEVEL_LEN );
        break;
      case UUID_CURRENT_ID:
        *pLen = CURRENT_LEN;
        VOID memcpy( pValue, pAttr->pValue, CURRENT_LEN );
        break;
      case UUID_BIKE_SPEED_ID:
        *pLen = BIKE_SPEED_LEN;
        VOID memcpy( pValue, pAttr->pValue, BIKE_SPEED_LEN );
        break;
      case UUID_BIKE_FLAGS_ID:
        *pLen = BIKE_FLAGS_LEN;
        VOID memcpy( pValue, pAttr->pValue, BIKE_FLAGS_LEN );
        break;
      default:
        // Should never get here! (characteristics 3 and 4 do not have read permissions)
        *pLen = 0;
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }
  }
  else
  {
    // 128-bit UUID
    *pLen = 0;
    status = ATT_ERR_INVALID_HANDLE;
  }

  return ( status );
}






/*********************************************************************
 * @fn      simpleProfile_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 */
static bStatus_t simpleProfile_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                            uint8 *pValue, uint8 len, uint16 offset,
                                            uint8 method )
{  
  bStatus_t status = SUCCESS;
  uint8 notifyApp = 0xFF;
  
  // If attribute permissions require authorization to write, return error
  if ( gattPermitAuthorWrite( pAttr->permissions ) )
  {
    // Insufficient authorization
    return ( ATT_ERR_INSUFFICIENT_AUTHOR );
  }
  if ( pAttr->type.len == ATT_BT_UUID_SIZE )
  {
    // 16-bit UUID
    uint16 uuid = BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch ( uuid )
    {
      case UUID_MODE_ID:
        if ( offset == 0 && len == MODE_LEN )
        {
          memcpy (pAttr->pValue, pValue, len );
          notifyApp = MODE_ID;
        }
        break;
        break;
      case UUID_PWM_DUTY_CYCLE_ID:
        if ( offset == 0 && len == PWM_DUTY_CYCLE_LEN )
        {
          memcpy (pAttr->pValue, pValue, len );
          notifyApp = PWM_DUTY_CYCLE_ID;
        }
        break;
      case UUID_V_THRESHOLD_ID:
        if ( offset == 0 && len == V_THRESHOLD_LEN )
        {
          memcpy (pAttr->pValue, pValue, len );
          notifyApp = V_THRESHOLD_ID;
        }
        break;
      case UUID_BIKE_SPEED_ID:
        if ( offset == 0 && len == BIKE_SPEED_LEN )
        {
          memcpy (pAttr->pValue, pValue, len );
          notifyApp = BIKE_SPEED_ID;
        }
        break;
      case UUID_BIKE_FLAGS_ID:
        if ( offset == 0 && len == BIKE_FLAGS_ID )
        {
          memcpy (pAttr->pValue, pValue, len );
          notifyApp = BIKE_FLAGS_ID;
        }
        break;
      case GATT_CLIENT_CHAR_CFG_UUID:
        status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                                 offset, GATT_CLIENT_CFG_NOTIFY );
        break;
        
      default:
        // Should never get here! (characteristics 2 and 4 do not have write permissions)
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }
  }
  else
  {
    // 128-bit UUID
    status = ATT_ERR_INVALID_HANDLE;
  }
  
  // If a charactersitic value changed then callback function to notify application of change
  if ( (notifyApp != 0xFF ) && simpleProfile_AppCBs && simpleProfile_AppCBs->pfnSimpleProfileChange )
  {
    simpleProfile_AppCBs->pfnSimpleProfileChange( notifyApp );  
  }
  
  return ( status );
}



/*********************************************************************
*********************************************************************/
