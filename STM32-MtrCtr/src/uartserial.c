/**
  ******************************************************************************

  ******************************************************************************
  * This is program is provided as is with no warranty!!
  ******************************************************************************
  */
#include "main.h"
//#include "pwm_ctr.h"

#define BUFFER_SIZE 32

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


//notice the use of the volatile keyword this is important as without it the compiler may make
//optimisations assuming the value of this variable hasnt changed
volatile char received_buffer[BUFFER_SIZE+1];
volatile char received_buffer2[BUFFER_SIZE+1];
volatile char Lastreceived_buffer[BUFFER_SIZE+1];

static uint8_t DataReceivedCounter = 0; //tracks the number of characters received so far, reset after each command
static uint8_t DataReceivedCounter2 = 0; //tracks the number of characters received so far, reset after each command
static uint8_t secondcharflag = 0;
int val = 0;


/* Private function prototypes -----------------------------------------------*/

void USARTCommandReceived(char * command);
void ClearCommand();
void Delay(int nCount);



//writes out a string to the passed in usart. The string is passed as a pointer
void UART_write(USART_TypeDef* USARTx, volatile char *s){

	while(*s){
		// wait until data register is empty
		while( !(USARTx->SR & 0x00000040) );
		USART_SendData(USARTx, *s);
		*(s++);
	}
}


//This method is executed when data is received on the RX line (this is the interrupt), this method can process the
//data thats been received and decide what to do. It is executed for each character received, it reads each character
//and checks to see if it received the enter key (ascii code 13) or if the total number of characters received is greater
//that the buffer size.
//Note that there is no reference to this method in our setup code, this is because the name of this method is defined in the
//startup_stm32f4xx.S (you can find this in the startup_src folder). When listening for interrupts from USART 2 or 3 you would
//define methods named USART2_IRQHandler or USART3_IRQHandler
void USART1_IRQHandler(void){
	//check the type of interrupt to make sure we have received some data.
	if( USART_GetITStatus(USART1, USART_IT_RXNE) ){
		char t = USART1->DR; //Read the character that we have received

		USART_SendData(USART1, t);

		/* Was it the end of the line? */

					if( t == '\n' || t == '\r' )
					{
						/* Just to space the output from the input. */
						UART_write(USART1,  "\r\n" );
						secondcharflag = 0;

						/* See if the command is empty, indicating that the last command
						is to be executed again. */
						if( DataReceivedCounter == 0 )
						{
							/* Copy the last command back into the input string. */
							strcpy( received_buffer, Lastreceived_buffer );
						}

						/* Pass the received command to the command interpreter.  The
						command interpreter is called repeatedly until it returns
						pdFALSE	(indicating there is no more output) as it might
						generate more than one string. */
						do
						{
							/* Get the next output string from the command interpreter. */
							USARTCommandReceived(received_buffer);
							/* Write the generated string to the UART. */
							//vSerialPutString( xPort, ( signed char * ) pcOutputString, strlen( pcOutputString ) );

						} while(0);

						/* All the strings generated by the input command have been
						sent.  Clear the input string ready to receive the next command.
						Remember the command that was just processed first in case it is
						to be processed again. */
						strcpy( Lastreceived_buffer, received_buffer );
						DataReceivedCounter = 0;
						DataReceivedCounter2 =0;
						memset( received_buffer, 0x00, BUFFER_SIZE );
						memset( received_buffer2, 0x00, BUFFER_SIZE );
						t = 0;

						UART_write(USART1,  "\r\n[Press ENTER to execute the previous command again]\r\n>" );;
					}

					else if( ( t == ' ') )
					{
						secondcharflag = 1;
					}
					else
					{

						if (secondcharflag == 0)

						{

							if( t == '\r' )
							{
								/* Ignore the character. */
							}

							else if( ( t == '\b' ) || ( t == 0x7F ) )
							{
								/* Backspace was pressed.  Erase the last character in the
								string - if any. */
								if( DataReceivedCounter > 0 )
								{
									DataReceivedCounter--;
									received_buffer[ DataReceivedCounter ] = '\0';
								}
							}
							else
							{
								/* A character was entered.  Add it to the string entered so
								far.  When a \n is entered the complete	string will be
								passed to the command interpreter. */
								if( ( t >= ' ' ) && ( t <= '~' ) )
								{
									if( DataReceivedCounter < 32 )
									{
										received_buffer[ DataReceivedCounter ] = t ;
										DataReceivedCounter++;
									}
								}
							}

						}

						else
						{

							if( t == '\r' )
							{
								/* Ignore the character. */
							}

							else if( ( t == '\b' ) || ( t == 0x7F ) )
							{
								/* Backspace was pressed.  Erase the last character in the
								string - if any. */
								if( DataReceivedCounter2 > 0 )
								{
									DataReceivedCounter2--;
									received_buffer2[ DataReceivedCounter2 ] = '\0';
								}
							}
							else
							{
								/* A character was entered.  Add it to the string entered so
								far.  When a \n is entered the complete	string will be
								passed to the command interpreter. */
								if( ( t >= ' ' ) && ( t <= '~' ) )
								{
									if( DataReceivedCounter2 < 32 )
									{
										received_buffer2[ DataReceivedCounter2 ] = t ;
										DataReceivedCounter2++;
									}
								}
							}

						}

						}








/*
		if( (DataReceivedCounter < BUFFER_SIZE) && t != 32 ){
			received_buffer[DataReceivedCounter] = t;
			DataReceivedCounter++;
		}
		else{ // otherwise reset the character counter and print the received string
			DataReceivedCounter = 0;
			//only raise a command event if the enter key was pressed otherwise just clear
			if(t == 13){
				USARTCommandReceived(received_buffer);
			}

			ClearCommand(); */

		}
	}


//this method is called when a command is received from the USART, a command is only received when enter
//is pressed, if the buffer length is exceeded the buffer is cleared without raising a command
void USARTCommandReceived(char * command){
	UART_write(USART1, received_buffer);

	if        (compare(command, "l5-on") == 0){
		STM_EVAL_LEDOn(LED5);
	}else 	if(compare(command, "l5-off") == 0){
		STM_EVAL_LEDOff(LED5);

	}else 	if(compare(command, "l6-on") == 0){
		STM_EVAL_LEDOn(LED6);
	}else 	if(compare(command, "l6-off") == 0){
		STM_EVAL_LEDOff(LED6);

	}else 	if(compare(command, "l3-on") == 0){
		STM_EVAL_LEDOn(LED3);
	}else 	if(compare(command, "l3-off") == 0){
		STM_EVAL_LEDOff(LED3);

	}else 	if(compare(command, "l4-on") == 0){
		STM_EVAL_LEDOn(LED4);
	}else 	if(compare(command, "l4-off") == 0){
		STM_EVAL_LEDOff(LED4);
	}else 	if(compare(command, "m-on") == 0){
		val = atoi(received_buffer2);
		pwm_initconfig(val);
		val = 0;
	}else 	if(compare(command, "m-off") == 0){
		pwm_deinitconfig();
	}

}
void ClearCommand(){
	int i =0;
	for(i=0;i < BUFFER_SIZE; i++){
		received_buffer[i] = 0;
	}

}

void Delay(int nCount) {
  while(nCount--) {
  }
}


void ts_itoa(char **buf, unsigned int d, int base)
{
	int div = 1;
	while (d/div >= base)
		div *= base;

	while (div != 0)
	{
		int num = d/div;
		d = d%div;
		div /= base;
		if (num > 9)
			*((*buf)++) = (num-10) + 'A';
		else
			*((*buf)++) = num + '0';
	}
}

int compare(char a[], char b[])
{
   int c = 0;

   while( a[c] == b[c] )
   {
      if( a[c] == '\0' || b[c] == '\0' )
         break;
      c++;
   }
   if( a[c] == '\0' && b[c] == '\0' )
      return 0;
   else
      return -1;
}

