SECTION	.data
EXTERN	inbound_queue	; (defined in main.C)
data		DB	0		; put rcvd byte here

SECTION	.text
ALIGN	16
BITS	32

BASE_PORT	EQU	3F8h		; we have this in our lab

LSR_PORT	EQU	BASE_PORT+5
RBR_PORT	EQU	BASE_PORT
THR_PORT	EQU	BASE_PORT

; ---------------------------------------------------------------------
; void SerialPut(char ch)
; ---------------------------------------------------------------------
; This function uses programmed waiting loop I/O
; to output the ASCII character 'ch' to the UART.

GLOBAL	SerialPut

SerialPut:						; (1) Wait for THRE = 1
mov dx, LSR_PORT 	; Fetch register for processing
in	al, dx 				;	Move stuff
and al, 0x20
cmp al, 0x20			;	Check if THRE bit is set
jne SerialPut			; Else loop back ;jnc

; (2) Output character to UART  (THR = Transmitter holding register)
mov dx, THR_PORT  ; Move THR_PORT adress to dx (Because out does not work with THR_PORT)
mov al, [esp+4]		; Get character from stack for transmission (return adress = 4*8 bits)
out dx, al				; Put character in dx (=THR_Port)

ret								; (3) Return to caller


; ---------------------------------------------------------------------
; void interrupt SerialISR(void)
; ---------------------------------------------------------------------
; This is an Interrupt Service Routine (ISR) for
; serial receive interrupts.  Characters received
; are placed in a queue by calling Enqueue(char).

GLOBAL	SerialISR
EXTERN	QueueInsert	; (provided by LIBPC)

SerialISR:	STI             	; Enable (higher-priority) IRQs

pushad	; (1) Preserve all registers

mov dx, LSR_PORT		; Save LSR_PORT adress in dx
in al, dx						; Read LSR_PORT to ax
and ax, 0x01
cmp ax, 0x01				; Check receiver buffer full
jne _Eoi						; if rbf  = 0, rbr not ready/ no data --> jmp end of interrupt

mov dx, RBR_PORT		; (2) Get character from UART
in al, dx 					;
mov [data], al			; (3) Put character into queue

push dword data			; Param #2: address of data
push dword [inbound_queue] ; Param #1: address of queue

CALL	QueueInsert
ADD	ESP,8

_Eoi:		; End Of Interrupt	; (4) Enable lower priority interrupts
; <your code here>			; (Send Non-Specific EOI to PIC)

mov ax, 0x20
mov dx, 0x20
out dx, ax

popad	; (5) Restore all registers
ret	; (6) Return to interrupted code
