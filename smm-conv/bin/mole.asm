;	org $7000	; ----------------------------------------

; block address: $7000

note_tables
	.by $F3 $E6 $D9 $CC $C1 $B5 $AD $A2 $99 $90 $88 $80 $79 $72 $6C $66 
	.by $60 $5B $55 $51 $4C $48 $44 $40 $3C $39 $35 $32 $2F $2D $2A $28 
	.by $25 $23 $21 $1F $1D $1C $1A $18 $17 $16 $14 $13 $12 $11 $10 $0F 
	.by $0E $0D $0C $0B $0A $09 $08 $07 $06 $05 $04 $03 $02 $01 $00 $00 
	.by $BF $B6 $AA $A1 $98 $8F $89 $80 $F2 $E6 $DA $CE $BF $B6 $AA $A1 
	.by $98 $8F $89 $80 $7A $71 $6B $65 $5F $5C $56 $50 $4D $47 $44 $3E 
	.by $3C $38 $35 $32 $2F $2D $2A $28 $25 $23 $21 $1F $1D $1C $1A $18 
	.by $17 $16 $14 $13 $12 $11 $10 $0F $0E $0D $0C $0B $0A $09 $08 $07 
	.by $FF $F1 $E4 $D8 $CA $C0 $B5 $AB $A2 $99 $8E $87 $7F $79 $73 $70 
	.by $66 $61 $5A $55 $52 $4B $48 $43 $3F $3C $39 $37 $33 $30 $2D $2A 
	.by $28 $25 $24 $21 $1F $1E $1C $1B $19 $17 $16 $15 $13 $12 $11 $10 
	.by $0F $0E $0D $0C $0B $0A $09 $08 $07 $06 $05 $04 $03 $02 $01 $00 
	.by $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 
	.by $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 
	.by $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 
	.by $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 $00 

;	org $7100	; ----------------------------------------

; block address: $7100

sfx_modes
	.by $02 $02 $02 $02 $02 $02 $02 $02 $02 $00 $00 $00 $00 $00 $00 $00 
	.by $00 $00 $00 

;	org $7113	; ----------------------------------------

; block address: $7113

sfx_note
	.by $00 $00 $00 $00 $00 $00 $00 $00 $80 $00 $00 $00 $00 $00 $00 $00 
	.by $00 $00 $00 

;	org $7126	; ----------------------------------------

; current block address: 7126

sfxptr
	dta a(data_sfx_0) ; offset in data 0000 optimized 1=>0
	dta a(data_sfx_1) ; offset in data 0014 optimized 2=>1
	dta a(data_sfx_2) ; offset in data 0022 optimized 3=>2
	dta a(data_sfx_3) ; offset in data 0062 optimized 5=>3
	dta a(data_sfx_4) ; offset in data 00A0 optimized 7=>4
	dta a(data_sfx_5) ; offset in data 00C4 optimized 8=>5
	dta a(data_sfx_6) ; offset in data 00E8 optimized 9=>6
	dta a(data_sfx_7) ; offset in data 00FE optimized 10=>7
	dta a(data_sfx_8) ; offset in data 0120 optimized 16=>8
	dta a(data_sfx_9) ; offset in data 0140 optimized 54=>9
	dta a(data_sfx_10) ; offset in data 014C optimized 55=>10
	dta a(data_sfx_11) ; offset in data 015C optimized 56=>11
	dta a(data_sfx_12) ; offset in data 0178 optimized 57=>12
	dta a(data_sfx_13) ; offset in data 01CC optimized 58=>13
	dta a(data_sfx_14) ; offset in data 01F0 optimized 59=>14
	dta a(data_sfx_15) ; offset in data 0204 optimized 60=>15
	dta a(data_sfx_16) ; offset in data 0224 optimized 61=>16
	dta a(data_sfx_17) ; offset in data 0230 optimized 62=>17
	dta a(data_sfx_18) ; offset in data 023C optimized 63=>18

;	org $714C	; ----------------------------------------

; current block address: 714C

tabptr
	dta a(data_tab_0) ; offset in data 0248 optimized 1=>0
	dta a(data_tab_1) ; offset in data 026A optimized 2=>1
	dta a(data_tab_2) ; offset in data 028C optimized 3=>2
	dta a(data_tab_3) ; offset in data 02AE optimized 4=>3
	dta a(data_tab_4) ; offset in data 02D0 optimized 5=>4
	dta a(data_tab_5) ; offset in data 02F2 optimized 6=>5
	dta a(data_tab_6) ; offset in data 0314 optimized 7=>6
	dta a(data_tab_7) ; offset in data 0336 optimized 8=>7
	dta a(data_tab_8) ; offset in data 0358 optimized 10=>8
	dta a(data_tab_9) ; offset in data 037A optimized 11=>9
	dta a(data_tab_10) ; offset in data 039C optimized 12=>10
	dta a(data_tab_11) ; offset in data 03BE optimized 13=>11
	dta a(data_tab_12) ; offset in data 03E0 optimized 14=>12
	dta a(data_tab_13) ; offset in data 0402 optimized 15=>13
	dta a(data_tab_14) ; offset in data 0424 optimized 16=>14
	dta a(data_tab_15) ; offset in data 0446 optimized 17=>15
	dta a(data_tab_16) ; offset in data 0468 optimized 18=>16
	dta a(data_tab_17) ; offset in data 048A optimized 21=>17
	dta a(data_tab_18) ; offset in data 04AC optimized 22=>18
	dta a(data_tab_19) ; offset in data 04CE optimized 23=>19
	dta a(data_tab_20) ; offset in data 04F0 optimized 24=>20
	dta a(data_tab_21) ; offset in data 0512 optimized 30=>21
	dta a(data_tab_22) ; offset in data 0534 optimized 31=>22
	dta a(data_tab_23) ; offset in data 0586 optimized 32=>23
	dta a(data_tab_24) ; offset in data 05D8 optimized 33=>24
	dta a(data_tab_25) ; offset in data 062A optimized 34=>25
	dta a(data_tab_26) ; offset in data 067C optimized 35=>26
	dta a(data_tab_27) ; offset in data 06CE optimized 37=>27
	dta a(data_tab_28) ; offset in data 0700 optimized 38=>28
	dta a(data_tab_29) ; offset in data 0732 optimized 39=>29
	dta a(data_tab_30) ; offset in data 075C optimized 63=>30

;	org $718A	; ----------------------------------------

; block address: $718A

song
	.by $80 $05 $40 $40 $00 $7F $08 $11 $04 $40 $08 $12 $03 $40 $08 $13 
	.by $06 $40 $15 $14 $00 $09 $08 $11 $04 $0A $08 $12 $03 $0B $08 $13 
	.by $06 $0C $15 $14 $00 $0D $08 $11 $04 $0E $08 $12 $03 $0F $08 $13 
	.by $06 $10 $15 $14 $84 $01 $05 $40 $00 $01 $08 $11 $04 $02 $08 $12 
	.by $03 $05 $08 $13 $06 $07 $15 $14 $84 $01 $0E $40 $82 $05 $40 $40 
	.by $80 $04 $40 $40 $16 $17 $1A $40 $FF $40 $40 $40 $80 $06 $40 $40 
	.by $18 $19 $40 $40 $FF $40 $40 $40 $80 $05 $40 $40 $1B $1C $1D $40 
	.by $82 $00 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 
	.by $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 
	.by $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 
	.by $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 
	.by $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 
	.by $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 
	.by $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 
	.by $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 
	.by $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $40 $1E $40 $40 $40 

