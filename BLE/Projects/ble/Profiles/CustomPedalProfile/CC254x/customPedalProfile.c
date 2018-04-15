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

#include "customPedalProfile.h"
        
#include "hal_key.h"
#include "hal_adc.h"
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

#define SERVAPP_NUM_ATTR_SUPPORTED      16

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
// Simple GATT Profile Service UUID: 0xFFF0
CONST uint8 customPedalProfileServUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SIMPLEPROFILE_SERV_UUID), HI_UINT16(SIMPLEPROFILE_SERV_UUID)
};

#define createUUID(name) CONST uint8 _ ## name[] = { LO_UINT16(name), HI_UINT16(name) }
createUUID(UUID_BATTERY_LV_ID);
createUUID(UUID_PEDALLING_STRENGTH_ID);
createUUID(UUID_CONFIG_ID);
createUUID(UUID_RAW_DATA_ID);

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

static customPedalProfileCBs_t *customPedalProfile_AppCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

// Simple Profile Service attribute 
// GATT_PROP_READ | GATT_PROP_WRITE | GATT_PROP_NOTIFY
static CONST gattAttrType_t customPedalProfileService = { ATT_BT_UUID_SIZE, customPedalProfileServUUID };

static uint8 _BATTERY_LV_Props = GATT_PROP_READ|GATT_PROP_NOTIFY;
static uint8 _BATTERY_LV_Value = 0;
static uint8 _BATTERY_LV_Desc[] = "Battery level";
static gattCharCfg_t* _BATTERY_LV_NotiConfig;

static uint8 _PEDALLING_STRENGTH_Props = GATT_PROP_READ|GATT_PROP_NOTIFY;
static float _PEDALLING_STRENGTH_Value = 0;
static uint8 _PEDALLING_STRENGTH_Desc[] = "Pedalling strength";
static gattCharCfg_t* _PEDALLING_STRENGTH_NotiConfig;

static uint8 _CONFIG_Props = GATT_PROP_READ|GATT_PROP_WRITE;
static uint32 _CONFIG_Value = 0;
static uint8 _CONFIG_Desc[] = "Configuration";

static uint8 _RAW_DATA_Props = GATT_PROP_READ|GATT_PROP_NOTIFY;
static uint16 _RAW_DATA_Value = 0;
static uint8 _RAW_DATA_Desc[] = "Raw pedalling data";
static gattCharCfg_t* _RAW_DATA_NotiConfig;

/*********************************************************************
 * Profile Attributes - Table
 */
#define ATT_Declaration(permission, props) {{ ATT_BT_UUID_SIZE, characterUUID }, permission, 0, props},
#define ATT_Value_Declaration(permission, props, UUID) {{ ATT_BT_UUID_SIZE, UUID }, permission, 0, (uint8*)props},
#define ATT_Desc_Declaration(permission, props) {{ ATT_BT_UUID_SIZE, charUserDescUUID }, permission, 0, props},
#define ATT_Noti_Declaration(props) {{ ATT_BT_UUID_SIZE, clientCharCfgUUID }, GATT_PERMIT_READ | GATT_PERMIT_WRITE, 0, (uint8 *)props},

static gattAttribute_t customPedalProfileAttrTbl[SERVAPP_NUM_ATTR_SUPPORTED] = 
{
  // Simple Profile Service
  { 
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8 *)&customPedalProfileService            /* pValue */
  },
  
     //   GATT_PERMIT_WRITE|GATT_PERMIT_READ, GATT_PERMIT_AUTHEN_READ | GATT_PERMIT_AUTHEN_WRITE, 
   ATT_Declaration(GATT_PERMIT_READ,          &_BATTERY_LV_Props)
   ATT_Value_Declaration(GATT_PERMIT_READ,    &_BATTERY_LV_Value, _UUID_BATTERY_LV_ID )
   ATT_Desc_Declaration(GATT_PERMIT_READ,     _BATTERY_LV_Desc)
   ATT_Noti_Declaration(&_BATTERY_LV_NotiConfig)
     
   ATT_Declaration(GATT_PERMIT_READ,          &_PEDALLING_STRENGTH_Props)
   ATT_Value_Declaration(GATT_PERMIT_READ,    &_PEDALLING_STRENGTH_Value, _UUID_PEDALLING_STRENGTH_ID )
   ATT_Desc_Declaration(GATT_PERMIT_READ,     _PEDALLING_STRENGTH_Desc)
   ATT_Noti_Declaration(&_PEDALLING_STRENGTH_NotiConfig)

   ATT_Declaration(GATT_PERMIT_READ,          &_CONFIG_Props)
   ATT_Value_Declaration(GATT_PERMIT_READ,    &_CONFIG_Value, _UUID_CONFIG_ID )
   ATT_Desc_Declaration(GATT_PERMIT_READ,     _CONFIG_Desc)


   ATT_Declaration(GATT_PERMIT_READ,          &_RAW_DATA_Props)
   ATT_Value_Declaration(GATT_PERMIT_READ,    &_RAW_DATA_Value, _UUID_RAW_DATA_ID )
   ATT_Desc_Declaration(GATT_PERMIT_READ,     _RAW_DATA_Desc)
   ATT_Noti_Declaration(&_RAW_DATA_NotiConfig)
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t customPedalProfile_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr, 
                                           uint8 *pValue, uint8 *pLen, uint16 offset,
                                           uint8 maxLen, uint8 method );
static bStatus_t customPedalProfile_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
                                            uint8 *pValue, uint8 len, uint16 offset,
                                            uint8 method );

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Simple Profile Service Callbacks
CONST gattServiceCBs_t customPedalProfileCBs =
{
  customPedalProfile_ReadAttrCB,  // Read callback function pointer
  customPedalProfile_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      customPedalProfile_AddService
 *
 * @brief   Initializes the Simple Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
bStatus_t customPedalProfile_AddService( uint32 services )
{
  uint8 status;
  
  // Allocate Client Characteristic Configuration table
  _BATTERY_LV_NotiConfig = (gattCharCfg_t *)osal_mem_alloc( sizeof(gattCharCfg_t) *
                                                              linkDBNumConns );
  _PEDALLING_STRENGTH_NotiConfig = (gattCharCfg_t *)osal_mem_alloc( sizeof(gattCharCfg_t) *
                                                              linkDBNumConns );
  _RAW_DATA_NotiConfig = (gattCharCfg_t *)osal_mem_alloc( sizeof(gattCharCfg_t) *
                                                              linkDBNumConns );
  if ( _BATTERY_LV_NotiConfig == NULL  || _PEDALLING_STRENGTH_NotiConfig == NULL  || _RAW_DATA_NotiConfig == NULL )
  {     
    return ( bleMemAllocError );
  }
  
 
  
  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, _BATTERY_LV_NotiConfig );
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, _PEDALLING_STRENGTH_NotiConfig );
  GATTServApp_InitCharCfg( INVALID_CONNHANDLE, _RAW_DATA_NotiConfig );
  
  if ( services & SIMPLEPROFILE_SERVICE )
  {
    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService( customPedalProfileAttrTbl, 
                                          GATT_NUM_ATTRS( customPedalProfileAttrTbl ),
                                          GATT_MAX_ENCRYPT_KEY_SIZE,
                                          &customPedalProfileCBs );
  }
  else
  {
    status = SUCCESS;
  }
  
  return ( status );
}

/*********************************************************************
 * @fn      customPedalProfile_RegisterAppCBs
 *
 * @brief   Registers the application callback function. Only call 
 *          this function once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
bStatus_t customPedalProfile_RegisterAppCBs( customPedalProfileCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    customPedalProfile_AppCBs = appCallbacks;
    
    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}

/*********************************************************************
 * @fn      customPedalProfile_SetParameter
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
bStatus_t customPedalProfile_SetParameter( uint8 param, uint8 len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case BATTERY_LV_ID:
      SetValue(uint8, _BATTERY_LV_Value)
     if ( GATTServApp_ProcessCharCfg( _BATTERY_LV_NotiConfig, ((uint8*)&_BATTERY_LV_Value), FALSE,
                                  customPedalProfileAttrTbl, GATT_NUM_ATTRS( customPedalProfileAttrTbl ),
                                  INVALID_TASK_ID, customPedalProfile_ReadAttrCB ) != SUCCESS)
       printf("WTF customPedalProfile_SetParameter\n");
      break;
      
    case PEDALLING_STRENGTH_ID:
      SetValue(uint8, _PEDALLING_STRENGTH_Value)
     if ( GATTServApp_ProcessCharCfg( _PEDALLING_STRENGTH_NotiConfig, ((uint8*)&_PEDALLING_STRENGTH_Value), FALSE,
                                  customPedalProfileAttrTbl, GATT_NUM_ATTRS( customPedalProfileAttrTbl ),
                                  INVALID_TASK_ID, customPedalProfile_ReadAttrCB ) != SUCCESS)
       printf("WTF customPedalProfile_SetParameter\n");
      break;
      
    case CONFIG_ID:
      SetValue(int32, _CONFIG_Value)
      break;
      
    case RAW_DATA_ID:
      SetValue(uint32, _RAW_DATA_Value);
      
     if ( GATTServApp_ProcessCharCfg( _RAW_DATA_NotiConfig, ((uint8*)&_RAW_DATA_Value), FALSE,
                                  customPedalProfileAttrTbl, GATT_NUM_ATTRS( customPedalProfileAttrTbl ),
                                  INVALID_TASK_ID, customPedalProfile_ReadAttrCB ) != SUCCESS)
       printf("WTF customPedalProfile_SetParameter\n");

      break;
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return ( ret );
}

/*********************************************************************
 * @fn      customPedalProfile_GetParameter
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
bStatus_t customPedalProfile_GetParameter( uint8 param, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case BATTERY_LV_ID:
      VOID memcpy( value, &_BATTERY_LV_Value, BATTERY_LV_LEN );
      break;
      
    case PEDALLING_STRENGTH_ID:
      VOID memcpy( value, &_PEDALLING_STRENGTH_Value, PEDALLING_STRENGTH_LEN );
      break;
      
    case CONFIG_ID:
      VOID memcpy( value, &_CONFIG_Value, CONFIG_LEN );
      break;
      
    case RAW_DATA_ID:
      VOID memcpy( value, &_RAW_DATA_Value, RAW_DATA_LEN );
      break;
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return ( ret );
}

/*********************************************************************
 * @fn          customPedalProfile_ReadAttrCB
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
static bStatus_t customPedalProfile_ReadAttrCB( uint16 connHandle, gattAttribute_t *pAttr, 
                                           uint8 *pValue, uint8 *pLen, uint16 offset,
                                           uint8 maxLen, uint8 method )
{
  
  //printf("ReadAttr\n");
    
     
     
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
      case UUID_BATTERY_LV_ID:
        *pLen = BATTERY_LV_ID;
        VOID memcpy( pValue, pAttr->pValue, BATTERY_LV_LEN );
        break;
      case UUID_PEDALLING_STRENGTH_ID:
        *pLen = PEDALLING_STRENGTH_LEN;
        VOID memcpy( pValue, pAttr->pValue, PEDALLING_STRENGTH_LEN );
        break;
      case UUID_CONFIG_ID:
        *pLen = CONFIG_LEN;
        VOID memcpy( pValue, pAttr->pValue, CONFIG_LEN );
        break;
      case UUID_RAW_DATA_ID:
        *pLen = RAW_DATA_LEN;
        VOID memcpy( pValue, pAttr->pValue, RAW_DATA_LEN );
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
 * @fn      customPedalProfile_WriteAttrCB
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
static bStatus_t customPedalProfile_WriteAttrCB( uint16 connHandle, gattAttribute_t *pAttr,
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
      case UUID_CONFIG_ID:
        if ( offset == 0 && len == 1 )
        {
          memcpy (pAttr->pValue, pValue, len );
          notifyApp = CONFIG_ID;
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
  if ( (notifyApp != 0xFF ) && customPedalProfile_AppCBs && customPedalProfile_AppCBs->pfncustomPedalProfileChange )
  {
    customPedalProfile_AppCBs->pfncustomPedalProfileChange( notifyApp );  
  }
  
  return ( status );
}



/*********************************************************************
*********************************************************************/
