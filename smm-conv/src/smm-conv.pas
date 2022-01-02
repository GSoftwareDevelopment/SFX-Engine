uses SysUtils;{$H+}
// {$I-}

// {$DEFINE SLOW}
type
	TTag = array[0..4] of byte;

{$I 'inc/const/smm-conv.inc'}

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

	SFXreduce:boolean = false;
	SFXreindex:boolean = false;
	TABreduce:boolean = false;
	TABreindex:boolean = false;

	sourceFN:string = '';
	outFN:string = '';
	outPath:string = '';
	confFN:string = '';
	resFN:string = '';
	orgFN:string = '';
	dataFN:string = DEFAULT_DATA_FILENAME;
	noteTabFN:string = DEFAULT_NOTE_TABLES_FILENAME;
	songTabFN:string = DEFAULT_SONG_TABLE_FILENAME;
	sfxmodesFN:string = DEFAULT_SFX_MODES_TABLE_FILENAME;
	sfxnotesFN:string = DEFAULT_SFX_NOTES_TABLE_FILENAME;
	sfxTabFN:string = DEFAULT_SFX_TABLE_FILENAME;
	tabTabFN:string = DEFAULT_TAB_TABLE_FILENAME;

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
{$i ./inc/const/stdout.inc}

procedure haltError(s:string);
begin
	writeLn(stderr,'ERROR: ',s);
	halt(-1);
end;

procedure init();
var i:byte;

begin
	fillbyte(data,10240,$ff);
	fillbyte(ndata,256,$ff);
	fillbyte(sfxptr,128,$ff);
	fillbyte(tabptr,128,$ff);
	for i:=0 to 63 do
	begin
		sfxUsage[i]:=-1;
		tabUsage[i]:=-1;
	end;
end;

procedure EOLStdOut(count:byte = 1);
begin
	if verbose=0 then exit;
	while count>0 do
	begin
		writeLn(stdout); dec(count);
	end;
end;

procedure writeStdOut(const s:string; const prm:array of const);
begin
	if verbose>0 then
		write(stdout,format(s,prm));
end;

procedure writeLnStdOut(const s:string; const prm:array of const);
begin
	if verbose>0 then
		writeLn(stdout,format(s,prm));
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

	if org=0 then
	begin
		org:=DEFAULT_ORIGIN;
		writeLnStdOut(STDOUT_ORIGIN_ADDRESS,[org]);
		EOLStdOut(2);
	end;

	if SFXReduce then ReduceSFX() else usedSFX:=64;
	if SFXReindex then ReindexSFX();

	if TABReduce then ReduceTAB() else usedTAB:=64;
	if TABReindex then ReindexTAB();

	saveASM(outFN,false);
end.