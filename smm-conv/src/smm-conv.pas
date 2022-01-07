uses SysUtils,SMMFile,SMMOptimize,BufFile;{$H+}
// {$I-}

const
	VERSION = '1.0.6';

// {$DEFINE SLOW}
{$I 'inc/const/smm-conv.inc'}

type
	PDataArray = ^TDataArray;
	TDataArray = array of PData;

var
	org:word = 0;
	temp_org:word;

	usedSFX,
	usedTAB:Byte;

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

	SMM:TSMMFile;

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

{$i ./inc/const/stdout.inc}
{$I ./inc/file_helpes.inc}

procedure haltError(s:string);
begin
	writeLn(stderr,'ERROR: ',s);
	halt(-1);
end;

{$I ./inc/save_asm.inc}
{$I ./inc/help.inc}
{$I ./inc/parseParams.inc}

begin
	// init();
	parseParams();

	SMM.init(sourceFN);
	SMM.load();

	if org=0 then
	begin
		org:=DEFAULT_ORIGIN;
		writeLnStdOut(STDOUT_ORIGIN_ADDRESS,[org]);
		EOLStdOut(2);
	end;

  if SFXReduce then usedSFX:=SMMReduceSFX(SMM) else usedSFX:=64;
	if SFXReindex then SMMReindexSFX(SMM);

	if TABReduce then usedTAB:=SMMReduceTAB(SMM) else usedTAB:=64;
  if TABReindex then SMMReindexTAB(SMM);

	writeLnStdOut('Generting files...',[]);
	saveASM(outFN);
end.
