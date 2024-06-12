#pragma once
/******************************************************************************/
/* File   : infSwcApplEcuMSwcServiceEcuM.h                                    */
/* Author : Nagaraja HULIYAPURADA-MATA                                        */
/* Date   : 01.02.1982                                                        */
/******************************************************************************/

/******************************************************************************/
/* #INCLUDES                                                                  */
/******************************************************************************/
#include "MemMap_SwcServiceEcuM.hpp"

/******************************************************************************/
/* #DEFINES                                                                   */
/******************************************************************************/

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

/******************************************************************************/
/* FUNCTIONS                                                                  */
/******************************************************************************/
extern FUNC(const Type_CfgSwcServiceEcuM_st*, SWCAPPLECUM_CODE) infSwcApplEcuMSwcServiceEcuM_stDetermineConfigurationPb (void);
extern FUNC(void,                             SWCAPPLECUM_CODE) infSwcApplEcuMSwcServiceEcuM_vSetInterruptsProgrammable (void);
extern FUNC(void,                             SWCAPPLECUM_CODE) infSwcApplEcuMSwcServiceEcuM_vInitDriverZero            (void);
extern FUNC(void,                             SWCAPPLECUM_CODE) infSwcApplEcuMSwcServiceEcuM_vCheckConsistency          (void);
extern FUNC(void,                             SWCAPPLECUM_CODE) infSwcApplEcuMSwcServiceEcuM_vSetSourceWakeup           (void);
extern FUNC(void,                             SWCAPPLECUM_CODE) infSwcApplEcuMSwcServiceEcuM_vSwitchOsModeApp           (void);
extern FUNC(void,                             SWCAPPLECUM_CODE) infSwcApplEcuMSwcServiceEcuM_vStartOS                   (void);

extern FUNC(void, SWCAPPLECUM_CODE) infSwcApplEcuMSwcServiceEcuM_vInitDriverOne(
   P2CONST(Type_CfgSwcServiceEcuM_st, AUTOMATIC, ECUM_APPL_CONST) ConfigPtr
);

/******************************************************************************/
/* EOF                                                                        */
/******************************************************************************/
