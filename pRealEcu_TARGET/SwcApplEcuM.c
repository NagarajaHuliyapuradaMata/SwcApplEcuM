/******************************************************************************/
/* File   : SwcApplEcuM.c                                                     */
/*                                                                            */
/* Author : Raajnaag HULIYAPURADA MATA                                        */
/*                                                                            */
/* License / Warranty / Terms and Conditions                                  */
/*                                                                            */
/* Everyone is permitted to copy and distribute verbatim copies of this lice- */
/* nse document, but changing it is not allowed. This is a free, copyright l- */
/* icense for software and other kinds of works. By contrast, this license is */
/* intended to guarantee your freedom to share and change all versions of a   */
/* program, to make sure it remains free software for all its users. You have */
/* certain responsibilities, if you distribute copies of the software, or if  */
/* you modify it: responsibilities to respect the freedom of others.          */
/*                                                                            */
/* All rights reserved. Copyright © 1982 Raajnaag HULIYAPURADA MATA           */
/*                                                                            */
/* Always refer latest software version from:                                 */
/* https://github.com/RaajnaagHuliyapuradaMata?tab=repositories               */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/* #INCLUDES                                                                  */
/******************************************************************************/
#include "Std_Types.hpp"

#include "Types_SwcServiceComM.hpp"
#include "Types_SwcServiceEcuM.hpp"
#include "CfgSwcServiceEcuM.hpp"

#include "EcuM_Prv.hpp"
#include "infMcalCanSwcApplEcuM.hpp"
#include "infMcalMcuSwcApplEcuM.hpp"
#include "infSwcServiceDcmSwcApplEcuM.hpp"
#include "infSwcServiceDetSwcApplEcuM.hpp"

#ifdef BUILD_RIVIAN
#include "infSwcServiceFiMSwcApplEcuM.hpp"
#endif

#include "infSwcServiceOsSwcApplEcuM.hpp"

//TBD: Clean-up
#include "CanManagerX.hpp"
#include "DemManagerX.hpp"
#include "EnvManagerX.hpp"
#include "CfgMcalMcu.hpp"
#include "Com.hpp"
#include "ComM.hpp"
#include "EcuM.hpp"
#include "Dem.hpp"
#include "CanTrcv.hpp"
#include "GpioX.hpp"

/******************************************************************************/
/* #DEFINES                                                                   */
/******************************************************************************/
//TBD: Move to CfgSwcServiceEcuM
#define CfgSwcServiceEcuM_dSizeFifoEventsEcu                                   4
#define CfgSwcServiceEcuM_dDelayReset                                         10

#ifdef DEBUG_FAST_SHUTDOWN
#define CfgSwcServiceEcuM_dTimeCoastModeMin                           (uint32)1
#else
#define CfgSwcServiceEcuM_dTimeCoastModeMin                           (uint32)10
#endif
#define CfgSwcServiceEcuM_dTimeCoastMode10ms (CfgSwcServiceEcuM_dTimeCoastModeMin * 60 * 100)

/******************************************************************************/
/* MACROS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* TYPEDEFS                                                                   */
/******************************************************************************/

/******************************************************************************/
/* CONSTS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* PARAMS                                                                     */
/******************************************************************************/

/******************************************************************************/
/* OBJECTS                                                                    */
/******************************************************************************/
static Type_SwcServiceEcuM_eEventsEcu aeFifoEventsEcu[CfgSwcServiceEcuM_dSizeFifoEventsEcu] = {
      SwcServiceEcuM_eEventEcu_None
   ,  SwcServiceEcuM_eEventEcu_None
   ,  SwcServiceEcuM_eEventEcu_None
   ,  SwcServiceEcuM_eEventEcu_None
};

static Type_SwcServiceEcuM_eModesEcu       eModeEcu            = SwcServiceEcuM_eModeEcu_Off;
static Type_SwcServiceEcuM_eTargetShutdown eTargetShutdown     = SwcServiceEcuM_eTargetShutdown_OFF;
static uint32                              u32TimerCoastMode   = 0;
static uint16                              u16TimerReset       = 0;
static uint16                              u16TimerShutdown    = 0;
static uint16                              u16TimerSleep       = 0;
static uint16                              u16TimerFblJump     = 0;
static uint16                              u16TimerSmartTester = 0;

/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/
extern void EcuM_ErrorHook(uint16 reason);

FUNC(void, SWCAPPLECUM_CODE) infSwcApplEcuMSwcServiceEcuM_vSetInterruptsProgrammable(void){
   Os_InitializeVectorTable();
}

FUNC(void, SWCAPPLECUM_CODE) infSwcApplEcuMSwcServiceEcuM_vInitDriverZero(void){
#if(ECUM_INCLUDE_DET == STD_ON)
   (void)infSwcServiceDetSwcApplEcuM_vInitFunction(0);
#endif
   (void)Dem_PreInit(0);
}

FUNC(const Type_CfgSwcServiceEcuM_st*, SWCAPPLECUM_CODE) infSwcApplEcuMSwcServiceEcuM_stDetermineConfigurationPb(void){
   return(CfgSwcServiceEcuM_capst[0]);
}

FUNC(void, SWCAPPLECUM_CODE) infSwcApplEcuMSwcServiceEcuM_vCheckConsistency(void){
   P2CONST(uint8,   AUTOMATIC, ECUM_APPL_CONST) adrTempPBHashPtr_u8;
       VAR(uint8,   AUTOMATIC                 ) cntrLoopCtr_u8;
       VAR(boolean, AUTOMATIC                 ) flgupdateValid = FALSE;
   if(
         NULL_PTR
      == SwcServiceEcuM_pcstCfg
   ){
      EcuM_ErrorHook(ECUM_E_NULL_PTR_RETURNED);
   }
   for(
      cntrLoopCtr_u8 = 0;
      cntrLoopCtr_u8 < CfgSwcServiceEcuM_dNumConfigSet;
      cntrLoopCtr_u8 ++
   ){
      if(
            SwcServiceEcuM_pcstCfg
         == CfgSwcServiceEcuM_capst[cntrLoopCtr_u8]
      ){
         flgupdateValid = TRUE;
         break;
      }
   }
   if(
         TRUE
      != flgupdateValid
   ){
      EcuM_ErrorHook(ECUM_E_INVALID_PTR_RETURNED);
   }
   adrTempPBHashPtr_u8 = &(SwcServiceEcuM_pcstCfg->au8HashPostBuild[0]);
   for(
      cntrLoopCtr_u8 = 0;
      cntrLoopCtr_u8 < CfgSwcServiceEcuM_dLenHash;
      cntrLoopCtr_u8 ++
   ){
      if(
            CfgSwcServiceEcuM_au8HashPreCompile[cntrLoopCtr_u8]
         != adrTempPBHashPtr_u8[cntrLoopCtr_u8]
      ){
         EcuM_ErrorHook(ECUM_E_CONFIGURATION_DATA_INCONSISTENT);
      }
   }
}

FUNC(void, SWCAPPLECUM_CODE) infSwcApplEcuMSwcServiceEcuM_vInitDriverOne(
   P2CONST(Type_CfgSwcServiceEcuM_st, AUTOMATIC, ECUM_APPL_CONST) ConfigPtr
){
   (void)infMcalCanSwcApplEcuM_vInitFunction();
   (void)CanIf_Init(ConfigPtr->stRefCfgModules.pcstCfgEcuabCanIf);
   (void)ComM_Init(ConfigPtr->stRefCfgModules.pcstCfgSwcServiceComM);
   (void)CanSM_Init(ConfigPtr->stRefCfgModules.pcstCfgSwcServiceCanSM);
   (void)PduR_Init(ConfigPtr->stRefCfgModules.pcstCfgSwcServicePduR);
   (void)Com_Init(ConfigPtr->stRefCfgModules.pcstCfgSwcServiceCom);
   (void)infSwcServiceDcmSwcApplEcuM_vInitFunction(0);
   (void)CanTp_Init(ConfigPtr->stRefCfgModules.pcstCfgSwcServiceCanTp);
   (void)Dem_Init();
#ifdef BUILD_RIVIAN
   (void)infSwcServiceFiMSwcApplEcuM_vInitFunction(ConfigPtr->stRefCfgModules.pcstCfgSwcServiceFiM);
#endif
}

FUNC(void, SWCAPPLECUM_CODE) infSwcApplEcuMSwcServiceEcuM_vSetSourceWakeup(void){
   VAR(Type_McalMcu_eResetReason, AUTOMATIC) leResetReason = McalMcu_eGetResetReason();
   VAR(uint8, AUTOMATIC) cntrLoopCtr_u8;

   if(
         McalMcu_eResetReason_Undefined
      != leResetReason
   ){
      for(
         cntrLoopCtr_u8 = 0;
         cntrLoopCtr_u8 < CfgSwcServiceEcuM_dNumSourceWakeup;
         cntrLoopCtr_u8 ++
      ){
         if(
               leResetReason
            == CfgSwcServiceEcuM_castListInfoSourceWakeUp[cntrLoopCtr_u8].ResetReason
         ){
            EcuM_SetWakeupEvent(
               CfgSwcServiceEcuM_castListInfoSourceWakeUp[cntrLoopCtr_u8].WakeupSourceId
            );
         }
      }
   }
}

FUNC(void, SWCAPPLECUM_CODE) infSwcApplEcuMSwcServiceEcuM_vSwitchOsModeApp(void){
}

FUNC(void, SWCAPPLECUM_CODE) infSwcApplEcuMSwcServiceEcuM_vStartOS(void){
   StartOS(SwcServiceEcuM_pcstCfg->tModeAppDefault)
}

void                                infSwcApplEcuMMcalMcu_vSetModeEcu (Type_SwcServiceEcuM_eModesEcu leModeEcu){       eModeEcu = leModeEcu;}
Type_SwcServiceEcuM_eModesEcu       infSwcApplEcuM_eGetModeEcu        (void)                                   {return eModeEcu;}
Type_SwcServiceEcuM_eTargetShutdown infSwcApplEcuM_eGetShutdownTarget (void)                                   {return eTargetShutdown;}


static void HandleTimerStart_vCoastMode    (void){u32TimerCoastMode = CfgSwcServiceEcuM_dTimeCoastMode10ms;}
static void HandleTimerStop_vCoastMode     (void){u32TimerCoastMode = 0;}
static void SwcApplEcuM_vSleep             (void){Dem_SetOperationCycleState(DemConf_DemOperationCycle_DemOperationCycle_POWER, DEM_CYCLE_STATE_END); EcuM_SelectShutdownTarget(SwcServiceEcuM_eTargetShutdown_OFF,   0); eTargetShutdown = SwcServiceEcuM_eTargetShutdown_SLEEP;        EcuM_GoDown(1);}
static void SwcApplEcuM_vShutdown          (void){Dem_SetOperationCycleState(DemConf_DemOperationCycle_DemOperationCycle_POWER, DEM_CYCLE_STATE_END); EcuM_SelectShutdownTarget(SwcServiceEcuM_eTargetShutdown_OFF,   0); eTargetShutdown = SwcServiceEcuM_eTargetShutdown_OFF;          EcuM_GoDown(1);}
static void SwcApplEcuM_vReset             (void){Dem_SetOperationCycleState(DemConf_DemOperationCycle_DemOperationCycle_POWER, DEM_CYCLE_STATE_END); EcuM_SelectShutdownTarget(SwcServiceEcuM_eTargetShutdown_RESET, 0); McalMcu_vPlaceResetRequest(McalMcu_eResetRequest_General);     EcuM_GoDown(1);}
static void SwcApplEcuM_vStartBootloader   (void){Dem_SetOperationCycleState(DemConf_DemOperationCycle_DemOperationCycle_POWER, DEM_CYCLE_STATE_END); EcuM_SelectShutdownTarget(SwcServiceEcuM_eTargetShutdown_RESET, 0); McalMcu_vPlaceResetRequest(McalMcu_eResetRequest_Bootloader);  EcuM_GoDown(1);}
static void SwcApplEcuM_vStartSmartTester  (void){Dem_SetOperationCycleState(DemConf_DemOperationCycle_DemOperationCycle_POWER, DEM_CYCLE_STATE_END); EcuM_SelectShutdownTarget(SwcServiceEcuM_eTargetShutdown_RESET, 0); McalMcu_vPlaceResetRequest(McalMcu_eResetRequest_SmartTester); EcuM_GoDown(1);}

void infSwcServiceEcuM_vSetEventEcu(
   Type_SwcServiceEcuM_eEventsEcu leEventEcu
){
   uint8 lu8IndexFifoEventsEcu = 0;
   while(
         (SwcServiceEcuM_eEventEcu_None != aeFifoEventsEcu[lu8IndexFifoEventsEcu])
      && (lu8IndexFifoEventsEcu < CfgSwcServiceEcuM_dSizeFifoEventsEcu)
   ){
      lu8IndexFifoEventsEcu++;
   }
   if(lu8IndexFifoEventsEcu < CfgSwcServiceEcuM_dSizeFifoEventsEcu){
      aeFifoEventsEcu[lu8IndexFifoEventsEcu] = leEventEcu;
   }
}

static Type_SwcServiceEcuM_eEventsEcu SwcServiceEcuM_eGetEventEcu(void){
   Type_SwcServiceEcuM_eEventsEcu leEventEcu = aeFifoEventsEcu[0];
   uint8                          lu8IndexFifoEventsEcu;
   if(leEventEcu != SwcServiceEcuM_eEventEcu_None){
      for(
         lu8IndexFifoEventsEcu = 0;
         lu8IndexFifoEventsEcu < (CfgSwcServiceEcuM_dSizeFifoEventsEcu - 1);
         lu8IndexFifoEventsEcu ++
      ){
         aeFifoEventsEcu[lu8IndexFifoEventsEcu] = aeFifoEventsEcu[lu8IndexFifoEventsEcu+1];
      }
      aeFifoEventsEcu[CfgSwcServiceEcuM_dSizeFifoEventsEcu-1] = SwcServiceEcuM_eEventEcu_None;
   }
   return leEventEcu;
}

static void SwcApplEcuM_vPrepareShutdown (uint16 ushTimeout){CANMGR_PduGroupStop(ComConf_ComIPduGroup_ComIPduGroup_Rx); CANMGR_PduGroupStop(ComConf_ComIPduGroup_ComIPduGroup_Tx); CANMGR_PduGroupStop(ComConf_ComIPduGroup_ComIPduGroup_Tx_AppCyclic); ComM_DeInit(); if(ushTimeout < 10){u16TimerShutdown = 10;}else{u16TimerShutdown = ushTimeout;}}
static void SwcApplEcuM_vPrepareSleep    (uint16 ushTimeout){CANMGR_PduGroupStop(ComConf_ComIPduGroup_ComIPduGroup_Rx); CANMGR_PduGroupStop(ComConf_ComIPduGroup_ComIPduGroup_Tx); CANMGR_PduGroupStop(ComConf_ComIPduGroup_ComIPduGroup_Tx_AppCyclic); ComM_DeInit(); if(ushTimeout < 10){u16TimerSleep    = 10;}else{u16TimerSleep    = ushTimeout;}}
extern void Env_ControlIgnitionByCoastmode(Type_SwcApplEcuM_eModeCoast leModeCoast);

static void SwcApplEcuM_vSwitchModeEcu(void){
   Type_SwcServiceEcuM_eEventsEcu leEventEcu = SwcServiceEcuM_eGetEventEcu();
   if(
         SwcServiceEcuM_eEventEcu_None
      != leEventEcu
   ){
      switch(
         eModeEcu
      ){
         case SwcServiceEcuM_eModeEcu_Quiet:
            switch(leEventEcu){
               case SwcServiceEcuM_eEventEcu_IgnitionOn:
                  HandleTimerStop_vCoastMode();
                  CANMGR_PduGroupStart(ComConf_ComIPduGroup_ComIPduGroup_Tx);
                  CANMGR_StartCommTimeout();
                  DemMGR_StartMonitoring();
                  Dem_SetOperationCycleState(
                        DemConf_DemOperationCycle_DemOperationCycle_IGNITION
                     ,  DEM_CYCLE_STATE_START
                  );
                  eModeEcu = SwcServiceEcuM_eModeEcu_Full;
                  break;
               case SwcServiceEcuM_eEventEcu_TimeoutComm:
                  CANMGR_StopCommTimeout();
                  eModeEcu = SwcServiceEcuM_eModeEcu_Monitor;
                  break;
               case SwcServiceEcuM_eEventEcu_TimeoutCoast:
                  Env_ControlIgnitionByCoastmode(SwcApplEcuM_eModeCoast_Over);
                  break;
               default:
                  break;
            }
            break;

         case SwcServiceEcuM_eModeEcu_Full:
            switch(leEventEcu){
               case SwcServiceEcuM_eEventEcu_IgnitionOff:
                  HandleTimerStart_vCoastMode();
                  Env_ControlIgnitionByCoastmode(SwcApplEcuM_eModeCoast_Start);
                  CANMGR_PduGroupStop(ComConf_ComIPduGroup_ComIPduGroup_Tx);
                  Dem_SetOperationCycleState(
                        DemConf_DemOperationCycle_DemOperationCycle_IGNITION
                     ,  DEM_CYCLE_STATE_END
                  );
                  DemMGR_StopMonitoring();
                  eModeEcu = SwcServiceEcuM_eModeEcu_Quiet;
                  break;
               case SwcServiceEcuM_eEventEcu_TimeoutComm:
                  Env_ControlIgnitionByCoastmode(SwcApplEcuM_eModeCoast_Over);
                  CANMGR_PduGroupStop(ComConf_ComIPduGroup_ComIPduGroup_Tx);
                  Dem_SetOperationCycleState(
                        DemConf_DemOperationCycle_DemOperationCycle_IGNITION
                     ,  DEM_CYCLE_STATE_END
                  );
                  DemMGR_StopMonitoring();
                  CANMGR_StartCommTimeout();
                  eModeEcu = SwcServiceEcuM_eModeEcu_Quiet;
                  break;
               case SwcServiceEcuM_eEventEcu_VehicleStop:
                  Env_ControlIgnitionByCoastmode(SwcApplEcuM_eModeCoast_Start);
                  break;
               case SwcServiceEcuM_eEventEcu_VehicleDrive:
                  Env_ControlIgnitionByCoastmode(SwcApplEcuM_eModeCoast_Stop);
                  break;
               default:
                  break;
            }
            break;

         case SwcServiceEcuM_eModeEcu_Wake:
            switch(leEventEcu){
               case SwcServiceEcuM_eEventEcu_WakeUpByCan:
                  CANMGR_SetFullCommunicationMode();
                  TRCV_Normal();
                  CANMGR_StartCommTimeout();
                  eModeEcu = SwcServiceEcuM_eModeEcu_Quiet;
                  break;
               case SwcServiceEcuM_eEventEcu_DeepStop:
                  SwcApplEcuM_vPrepareSleep(1U);
                  eModeEcu = SwcServiceEcuM_eModeEcu_Off;
                  break;
               case SwcServiceEcuM_eEventEcu_Shutdown:
                  SwcApplEcuM_vPrepareShutdown(10U);
                  eModeEcu = SwcServiceEcuM_eModeEcu_Off;
                  break;
               default:
                  break;
            }
            break;

         case SwcServiceEcuM_eModeEcu_Monitor:
            switch(leEventEcu){
               case SwcServiceEcuM_eEventEcu_WakeUpByCan:
                  CANMGR_StartCommTimeout();
                  eModeEcu = SwcServiceEcuM_eModeEcu_Quiet;
                  break;
               case SwcServiceEcuM_eEventEcu_DeepStop:
                  SwcApplEcuM_vPrepareSleep(1U);
                  eModeEcu = SwcServiceEcuM_eModeEcu_Off;
                  break;
               case SwcServiceEcuM_eEventEcu_Shutdown:
                  SwcApplEcuM_vPrepareShutdown(10U);
                  eModeEcu = SwcServiceEcuM_eModeEcu_Off;
                  break;
               case SwcServiceEcuM_eEventEcu_TimeoutCoast:
                  Env_ControlIgnitionByCoastmode(SwcApplEcuM_eModeCoast_Over);
                  break;
               default:
                  break;
            }
            break;

         case SwcServiceEcuM_eModeEcu_Off:
         default:
            break;
      }
   }
}

void infSwcApplEcuMSwcApplDcm_vPrepareReset           (uint16 lu16Timeout){if(lu16Timeout < CfgSwcServiceEcuM_dDelayReset){u16TimerReset       = CfgSwcServiceEcuM_dDelayReset;}else{u16TimerReset       = lu16Timeout;}}
void infSwcApplEcuMSwcApplDcm_vPrepareBootloaderJump  (uint16 lu16Timeout){if(lu16Timeout < CfgSwcServiceEcuM_dDelayReset){u16TimerFblJump     = CfgSwcServiceEcuM_dDelayReset;}else{u16TimerFblJump     = lu16Timeout;}}
void infSwcApplEcuMSwcApplDcm_vPrepareSmartTesterJump (uint16 lu16Timeout){if(lu16Timeout < CfgSwcServiceEcuM_dDelayReset){u16TimerSmartTester = CfgSwcServiceEcuM_dDelayReset;}else{u16TimerSmartTester = lu16Timeout;}}

void infSwcApplEcuMSwcServiceSchM_vMainFunction(void){
        if(u16TimerShutdown    > 0){--u16TimerShutdown;    if(0 == u16TimerShutdown    ){SwcApplEcuM_vShutdown();}}
   else if(u16TimerSleep       > 0){--u16TimerSleep;       if(0 == u16TimerSleep       ){SwcApplEcuM_vSleep();}}
   else if(u16TimerReset       > 0){--u16TimerReset;       if(0 == u16TimerReset       ){SwcApplEcuM_vReset();}}
   else if(u16TimerFblJump     > 0){--u16TimerFblJump;     if(0 == u16TimerFblJump     ){SwcApplEcuM_vStartBootloader();}}
   else if(u16TimerSmartTester > 0){--u16TimerSmartTester; if(0 == u16TimerSmartTester ){SwcApplEcuM_vStartSmartTester();}}
   else if(u32TimerCoastMode   > 0){--u32TimerCoastMode;   if(0 == u32TimerCoastMode   ){infSwcServiceEcuM_vSetEventEcu(SwcServiceEcuM_eEventEcu_TimeoutCoast);}}
   else{}
   CANMGR_TickCommTimeout();
   SwcApplEcuM_vSwitchModeEcu();
}

/******************************************************************************/
/* EOF                                                                        */
/******************************************************************************/
