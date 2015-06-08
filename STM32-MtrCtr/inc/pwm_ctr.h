/*
 * pwm_ctr.h
 *
 *  Created on: May 21, 2015
 *      Author: patels
 */

#ifndef PWM_CTR_H_
#define PWM_CTR_H_

#include <stm32f4xx.h>
#include <stm32f4xx.h>
#include <misc.h>
#include <stm32f4xx_usart.h>
#include "stm32f4_discovery.h"
#include <stdio.h>
#include <stdint.h>



int buff = 55 ;
int dataRX;
int offset = 0 ;
int MSBdataRX = 0;
int dataAdd = 0;
int i = 0 ;


int pwm_initconfig(int);
int pwm_deinitconfig(void);
int setpwm_freq_dutycycle(int,int);
void CODEUR_Reset(void);
void CODEUR_Read(void);


#endif /* PWM_CTR_H_ */
