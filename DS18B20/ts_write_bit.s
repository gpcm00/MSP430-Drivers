; Function:		tsWriteBit
;
; Author:		Gian Moreira
;
;
;
; Inputs:		byte - 1 byte to be send
;
; Outputs:		none
;
; Return:		R12 is register used to transmit the byte, and the register used to return a value for this function, so if
; 			this function fails, it wil return an unpredictable value; however, I haven't encountered any error so far
;------------------------------------------------------------------------------------------------------------------------------
        	    .cdecls C,LIST,"msp430.h"   		 	; Include device header file
        	    .cdecls C,LIST,"DS18B20.h"			   	; Include D1S8B20 header file
;------------------------------------------------------------------------------------------------------------------------------
; Register definitions
;------------------------------------------------------------------------------------------------------------------------------
        	    .define R12, byte						; R12 is also the register with the address of the keypad data
        	    .define R13, oneByteReg					; R13 holds the number of iterations needed
		    .define R14, int_ret_reg					; R14 keeps the value used to count the amount of iterations needed
;------------------------------------------------------------------------------------------------------------------------------
; Define functions constants
;------------------------------------------------------------------------------------------------------------------------------
ONE_BYTE 		.equ	8								; 8-bits
CYCLE_DELAY		.equ	18								; CYCLE_DELAY = ([63 cycles] - [9 cycles])/([3 cycles per iteration])
;------------------------------------------------------------------------------------------------------------------------------
; Code Section
;------------------------------------------------------------------------------------------------------------------------------
				.text
				.global tsWriteBit				; declare tsWriteByte as global

tsWriteBit:
				push	int_ret_reg				; save contents of R14

send_data:
				mov.b	#CYCLE_DELAY, int_ret_reg		; number of cycles needed to delay 60us
				rrc.b	byte					; shift the byte to be sent
				jc	send_H					; if the carry flag is up, send a high

send_L:				bis.b	#TS_OUTBIT, &TS_OUT			; [cycles: 4] pull the bus low and keep it low if no carry bit
				nop						; [cycles: 1] add an extra cycle to match send_H
				nop						; [cycles: 1] add an extra cycle to match send_H
				nop						; [cycles: 1] add an extra cycle to match send_H
				jmp	delay_loop				; [cycles: 2] jump to delay

send_H:				bis.b	#TS_OUTBIT, &TS_OUT			; [cycles: 4] pull the bus low to send a wakeup signal
				bic.b	#TS_OUTBIT, &TS_OUT			; [cycles: 4] release the bus
				nop						; [cycles: 1] add an extra cycle to match exactly 63 cycles

delay_loop:			dec	int_ret_reg				; [cycles: 1] decrement interation register
				jnz	delay_loop				; [cycles: 2] keep looping unitl interation register is 0

return_back:			pop	int_ret_reg				; restore whatever was stored in R14

				reta
