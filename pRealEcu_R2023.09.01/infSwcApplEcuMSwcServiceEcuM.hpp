#pragma once
/******************************************************************************/
/* File   : infSwcApplEcuMSwcServiceEcuM.h                                    */
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
#include "CompilerCfg_SwcServiceEcuM.hpp"

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
