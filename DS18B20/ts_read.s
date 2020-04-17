; Function:		tsReadData
;
; Author:		Gian Moreira
;
; Description:		. This function reads a n-bytes using Maxim Integrated 1-wire bus protocol
;
; Inputs:		bufLen	-	amount of bytes to be read
;
; Outputs:		populates the input address with the amount of bytes
;
; Return:		R12 is register used to pass in the address of the buffer, and the register used to return a value for this
; 			function, so if this function fails, it wil return an unpredictable value; however, I haven't encountered any
; 			error so far
;------------------------------------------------------------------------------------------------------------------------------
        	    .cdecls C,LIST,"msp430.h"   		 			; Include device header file
        	    .cdecls C,LIST,"DS18B20.h"			   			; Include D1S8B20 header file
;------------------------------------------------------------------------------------------------------------------------------
; Register definitions
;------------------------------------------------------------------------------------------------------------------------------
        	    .define R12, byte							; R12 is also the register with the address of the keypad data
        	    .define R13, bufLen							; R13 is a passed in argument with the size of the buffer
        	    .define R14, oneByteReg						; R14 holds the number of iterations needed
		    .define R15, int_ret_reg						; R15 keeps the value used to count the amount of iterations needed
;------------------------------------------------------------------------------------------------------------------------------
; Define functions constants
;------------------------------------------------------------------------------------------------------------------------------
ONE_BYTE 		.equ	8							; 8-bits
CYCLE_DELAY		.equ	11							; CYCLE_DELAY = ([63 cycles] - [30 cycles])/([3 cycles per iteration])
;------------------------------------------------------------------------------------------------------------------------------
; Code Section
;------------------------------------------------------------------------------------------------------------------------------
				.text
				.global tsReadData					; declare tsWriteByte as global

tsReadData:
				push	byte						; save the contents inside R12
				push	bufLen						; save the contents inside R13
				push	oneByteReg					; save the contents inside R14
				push	int_ret_reg					; save the contents inside R15

next_cycle:			mov.b	#ONE_BYTE, oneByteReg				; [cycles: 2] move one byte to R13 to keep track of the number of iterations

read_data:			rra.b	0(byte)						; [cycles: 4] shift the contents to the right since data is lsb first
				bis.b	#TS_OUTBIT, &TS_OUT				; [cycles: 4] pull the bus low to send a wakeup signal
				bic.b	#TS_OUTBIT, &TS_OUT				; [cycles: 4] release the bus

; since the toggle takes more than 8 cycles, it is okay to read right after the falling edge pusle, but since this could be in
; a high capacitance system, we will add 2 extra cycles, which is about 1.9us to allow the signal to be stable
; the signal should be read within 15us after the falling edge, which translates to less than 15 cycles for the msp430f5529

				mov.b	#TS_CYCLE_DELAY_R, int_ret_reg			; [cycles: 2] move the number of delay cycles needed to delay

				bit.b	#TS_INBIT, &TS_BUS				; [cycles: 3] check the status of the bus
				jnz		read_H					; [cycles: 2] if the comparisson returns a 0, read the signal as a low

read_L:				bic.b	#BIT7, 0(byte)					; [cycles: 5] set msb low if the input data is a '0'
				nop							; [cycles: 1] add an extra cycle to match read_H
				jmp		delay_loop				; [cycles: 2] jump to delay loop


read_H:				bis.b	#BIT7, 0(byte)					; [cycles: 5] set the msb high if the input data is a '1'
				nop							; [cycles: 1] add an extra cycle to match read_H
				nop							; [cycles: 1] add an extra cycle to match read_H
				nop							; [cycles: 1] add an extra cycle to make delay cycles a multiple of 3

delay_loop:			dec		int_ret_reg				; [cycles: 1] decrement interation register
				jnz		delay_loop				; [cycles: 2] keep looping unitl interation register is 0

recover:			dec.b	oneByteReg					; [cycles: 1] decrement the oneByte register to keep track of the number of bytes read
				jnz		read_data				; [cycles: 2] jump to read_data to read the next bit

prepare_next:	inc		byte							; [cycles: 1] increment cycle to store the next byte in the next address
				dec		bufLen					; [cycles: 1] decrement the length of the buffer until it reads 0
				jnz		next_cycle				; [cycles: 2] jump to get the next data

				pop		int_ret_reg				; restore whatever was stored inside R15
				pop		oneByteReg				; restore whatever was stored inside R14
				pop		bufLen					; restore whatever was stored inside R13

				pop		byte					; return the first address of byte


				reta

				.end



