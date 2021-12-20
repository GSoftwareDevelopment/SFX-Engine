uses SysUtils;
{$I-}

type
	TTag = array[0..4] of byte;

const
	EOL = #10#13;

	SFXMM_VER1_1 = $11;
	SFXMM_VER1_2 = $12;

	maxSFXs = 64;
	maxTABs = 64;
	maxNoteTables = 4;

	SFXNameLength = 14;
	TABNameLength = 8;
	SONGNameLength = 32;
	NOTETABnameLength = 11;

	section_main   :TTag = (83, 70, 88, 77, 77); // SFXMM
	section_SFX    :TTag = ( 0,  0, 83, 70, 88); // __SFX
	section_NOTE   :TTag = ( 0, 78, 79, 84, 69); // _NOTE
	section_TAB    :TTag = ( 0,  0, 84, 65, 66); // __TAB
	section_SONG   :TTag = ( 0, 83, 79, 78, 71); // _SONG

	CONFIG_FILENAME = 'sfx_engine.conf.inc';
	RESOURCE_FILENAME = 'resource.rc';

	DEFAULT_ORIGIN = $A000;

var
	song:array[0..255] of byte;
	sfxmodes,
	sfxnote:array[0..63] of byte;
	notetabs:array[0..255] of byte;
	sfxnames,
	tabnames:array[0..63] of string[16];
	sfxptr,
	tabptr,
	sfxSizes,
	tabSizes:array[0..63] of word;
	data:array[0..10239] of byte;

	org:word = 0;
	temp_org:word;
	topptr:word=0;
	totalSFX,
	totalTAB:byte;
	usedSFX,
	usedTAB:byte;
	SFXUsage,
	TABUsage:array[0..63] of shortint;

	ndata:array[0..255] of byte;
	ntopptr:word;

	optimizeSFX:boolean = false;
	SFXreindex:boolean = false;
	optimizeTAB:boolean = false;
	TABreindex:boolean = false;

	sourceFN:string = '';
	outFN:string = '';
	confFN:string = '';
	resFN:string = '';

	verbose:byte = 1;

//
//

	AUDIO_BUFFER_ADDR:word	= $E8;
	SFX_REGISTERS:word 			= $F0;
	SFX_CHANNELS_ADDR:word	= $6C0;

	SONG_ADDR:word					= 0;	// table of SONG definition
	SFX_MODE_SET_ADDR:word	= 0;	// table of SFX modes
	SFX_NOTE_SET_ADDR:word	= 0;	// table of SFX note table presets

	NOTE_TABLE_PAGE:byte		= 0;
	NOTE_TABLE_ADDR:word		= 0;	// predefined table of note frequency

	SFX_TABLE_ADDR:word			= 0;	// list for SFX definitions
	TAB_TABLE_ADDR:word			= 0;	// list for TAB definitions

	SFX_DATA_ADDR:word			= 0;

{$i ./inc/tfile.inc}

procedure haltError(s:string);
begin
	writeLn(stderr,'ERROR: ',s);
	halt(-1);
end;

procedure init();
begin
	if verbose>0 then writeLn('SFX Music Maker converter V1.0 by: GSD 2021');
	fillbyte(data,10240,$ff);
	fillbyte(ndata,256,$ff);
	fillbyte(sfxptr,128,$ff);
	fillbyte(tabptr,128,$ff);
end;

{$I ./inc/load_smm.inc}
{$I ./inc/save_asm.inc}
{$I ./inc/optimize.inc}
{$I ./inc/help.inc}
{$I ./inc/parseParams.inc}

begin
	init();
	parseParams();

	loadSMM(sourceFN);

	if verbose>0 then
	begin
		if org=0 then
		begin
			org:=DEFAULT_ORIGIN;
			writeLn('Origin address: ',hexstr(org,4));
			writeLn();
		end;
	end;

	if optimizeSFX then
	begin
//		writeLn('SFX Optimalization...');
		SFXScanUsage();
		SFXOptimize();
		if verbose>0 then writeLn();
	end
	else
		usedSFX:=64;

	if optimizeTAB then
	begin
//		writeLn('TAB Optimalization...');
		TABScanUsage();
		TABOptimize();
		if verbose>0 then writeLn();
	end
	else
		usedTAB:=64;
	saveASM(outFN,false);
end.
