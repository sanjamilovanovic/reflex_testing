
					.cdecls C,LIST,"msp430.h"       ; Include device header file

					.ref	Points
					.ref	Random_number
					.ref    Random_numberLED
					.ref    First_pressed
					.ref    Pressed

; PORT2 ISR
					.text
P2ISR				push.w  R9
					mov.w	#0xfff,R9
debounce:			dec		R9								; debounce time
					jnz		debounce
					bit.b	Random_number,&P2IFG			; check if button flag corresponds with current LED on
					jz		losing_points
					bit.b   #0x00,&P4OUT					; check if any LED on
					jnz     losing_points
					bit.b	Random_number,&P2IN				; check if button still pressed
					jz		p2isr_exit
					bit.b   #BIT0,First_pressed             ; check if button already pressed once while LED on
					jnz     p2isr_exit
					add.w   #3,Points						; add three points
					bic.b   Random_numberLED,&P4OUT			; turn off LED
					bis.b   #BIT0,First_pressed				; set First_pressed
					bis.b   #BIT0,Pressed					; a button was pressed
					jmp		p2isr_exit

losing_points:		bit.b	#BIT4,&P2IFG				; check if flag set
					jz 		testing2
        			bit.b	#BIT4,&P2IN					; check if button still pressed
					jz		p2isr_exit
					sub.w   #1,Points
					bic.b   Random_numberLED,&P4OUT		; if LED on turn off LED
					bis.b   #BIT0,Pressed				; a button was pressed
testing2:			bit.b	#BIT5,&P2IFG				; check if flag set
					jz 		testing3
        			bit.b	#BIT5,&P2IN					; check if button still pressed
					jz		p2isr_exit
					sub.w   #1,Points
					bic.b   Random_numberLED,&P4OUT		; if LED on turn off LED
					bis.b   #BIT0,Pressed				; a button was pressed
testing3:			bit.b	#BIT6,&P2IFG				; check if flag set
					jz 		testing4
        			bit.b	#BIT6,&P2IN					; check if button still pressed
					jz		p2isr_exit
					sub.w   #1,Points
					bic.b   Random_numberLED,&P4OUT		; if LED on turn off LED
					bis.b   #BIT0,Pressed				; a button was pressed
testing4:			bit.b	#BIT7,&P2IFG				; check if flag set
					jz 		p2isr_exit
        			bit.b	#BIT7,&P2IN					; check if button still pressed
					jz		p2isr_exit
					sub.w   #1,Points
					bic.b   Random_numberLED,&P4OUT		; if LED on turn off LED
					bis.b   #BIT0,Pressed				; a button was pressed
p2isr_exit:			bic.b	#0xff,&P2IFG         	    ; clear all flags
					pop.w	R9							; restore R9
					reti

; Vectors
					.sect	.int42
					.short	P2ISR

