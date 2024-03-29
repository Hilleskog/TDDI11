;
;  Test cases. All numbers are in hex.
;
;      111122223333 *     555566667777 =          5B061D958BF0ECA7C0481B5
;  3456FEDCAAAA1000 * EDBA00112233FF01 = 309A912AF7188C57E62072DD409A1000
;  FFFFEEEEDDDDCCCC * BBBBAAAA99998888 = BBBB9E2692C5DDDCC28F7531048D2C60
;  FFFFFFFFFFFFFFFF * FFFFFFFFFFFFFFFF = FFFFFFFFFFFFFFFE0000000000000001
;  00000001FFFFFFFF * 00000001FFFFFFFF = 0000000000000003FFFFFFFC00000001
;  FFFEFFFFFFFFFFFF * FFFF0001FFFFFFFF = FFFE0002FFFDFFFE0001FFFE00000001
;

;  void llmultiply(unsigned long long int l1,
;                  unsigned long long int l2,
;                  unsigned char *result);

;  a = AH * (2 << 32) + AL
;  b = BH * (2 << 32) + BL
;
;  a * b = (AH * BH            * (2 << 64)) +
;          (AH * BL + AL * BH) * (2 << 32)  +
;           AL * BL

; For more infromation, see the lab manual!

	SECTION .data



	SECTION .text
	ALIGN	16
	BITS	32

	;; To access parameter BH on the stack you do
	;;   MOV EAX, [EBP + 20]
	;; Replace the zero below with the correct value to be able
	;; to write your assembly code a little bit more readable:
	;;   MOV EAX, [EBP + BH_OFF]
AL_OFF	EQU     8	; Offset from EBP to low  bits of a (AL)
AH_OFF	EQU     12	; Offset from EBP to high bits of a (AH)
BL_OFF	EQU     16	; Offset from EBP to low  bits of b (BL)
BH_OFF	EQU     20	; Offset from EBP to high bits of b (BH)
RES_OFF	EQU     24	; Offset from EBP to result array pointer
	;;    ^^^^^ Replace 0 with correct values above

	GLOBAL llmultiply

llmultiply:
	PUSH EBP
	MOV EBP, ESP

	;; Put your implementation here
  ;; Result ESI:EDI
	;; eax*edx --> edx:eax

	mov esi, [EBP + RES_OFF]

	mov eax, 0
	mov [esi], eax
	mov [esi + 4], eax
	mov [esi + 8], eax
	mov [esi + 12], eax

  ;; AL * BL
  mov eax, [EBP + AL_OFF]
  mov edx, [EBP + BL_OFF]
	mul edx
		mov [esi], eax 			;; (AL*BL)L
		mov [esi + 4], edx 	;; (AL*BL)H

	;; AL * BH
	mov eax, [EBP + AL_OFF]
	mov edx, [EBP + BH_OFF]
	mul edx
		add [esi + 4], eax 	;; (AL*BH)L
		adc [esi + 8], edx 	;; (AL*BH)H
		adc [esi + 12], dword 0 ;; Carry

	;; AH * BL
	mov eax, [EBP + AH_OFF]
	mov edx, [EBP + BL_OFF]
	mul edx
		add [esi + 4], eax 	;; (AH*BL)L
		adc [esi + 8], edx 	;; (AH*BL)H
		adc [esi + 12], dword 0

	;; AH * BH
	mov eax, [EBP + AH_OFF]
	mov edx, [EBP + BH_OFF]
	mul edx
		add [esi + 8], eax 	;; (AH*BH)L
		adc [esi + 12], edx ;; (AH*BL)H


	POP EBP				; restore EBP reg
	RET				;  return
