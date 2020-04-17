        	    .cdecls C,LIST,"msp430.h"   		 	; Include device header file
        	    .cdecls C,LIST,"DS18B20.h"			   	; Include D1S8B20 header file
;------------------------------------------------------------------------------------------------------------------------------
; Register definitions
;------------------------------------------------------------------------------------------------------------------------------
        	    .define R12, return					; R12 is also the register with the address of the keypad data
        	    .define R13, int_ret_reg				; R13 keeps the value used to count the amount of iterations needed
;------------------------------------------------------------------------------------------------------------------------------
; Define functions constants
;------------------------------------------------------------------------------------------------------------------------------
CYCLE_DELAY		.equ	17
;------------------------------------------------------------------------------------------------------------------------------
; Code Section
;------------------------------------------------------------------------------------------------------------------------------
				.text
				.global tsReadBit						; declare tsWriteByte as global

tsReadBit:
				push	int_ret_reg						; save the contents of R13
				mov	#CYCLE_DELAY, int_ret_reg				; set the amount of cycles needed to be delayed for one bit transfer


				bis.b	#TS_OUTBIT, &TS_OUT					; [cycles: 4] pull the bus low to send a wakeup signal
				bic.b	#TS_OUTBIT, &TS_OUT					; [cycles: 4] release the bus

				nop								; [cycles: 1] delay one cycle to alow the bus to be stable
				nop								; [cycles: 1] delay another cycle allow the bus to stabilize
	
				bit.b	#TS_INBIT, &TS_BUS					; [cycles: 3] check if the bus was high or low
				jnz	read_H							; [cycles: 2] if the 0 flag isn't up, bus was high

read_L:				clr	return							; [cycles: 1] return a 0 if the bus was low

				nop								; [cycles: 1] delay by 2 cycles to make the number of cycles a multiple of 63
				nop								; [cycles: 1] delay by 2 cycles to make the number of cycles a multiple of 63

				jmp	delay_loop						; [cycles: 2] delay the period of a bit

read_H:				mov	#1, return						; [cycles: 1] return a 1 if the bus was high
				nop								; [cycles: 1] delay one cycle to match read_L
				nop								; [cycles: 1] delay one cycle to match read_L

				nop								; [cycles: 1] delay by 2 cycles to make the number of cycles a multiple of 63
				nop								; [cycles: 1] delay by 2 cycles to make the number of cycles a multiple of 63


delay_loop:			dec	int_ret_reg						; [cycles: 1] decrement interation register
				jnz	delay_loop						; [cycles: 2] keep delaying until count = 0

				pop	int_ret_reg						; restore R13

				reta
