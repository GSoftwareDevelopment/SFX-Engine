uses SysUtils;
{$I-}

type
	TTag = array[0..4] of byte;

const
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

	org:word=$a000;
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

	sourceFN,
	outFN,
	confFN:string;

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

{$I ./inc/load_smm.inc}

procedure saveASM(fn:string);
var
	f,fconf:text;

	procedure generateByLines(labelname:string; var ary:array of byte; len:word);
	var
		ofs:word;

	begin
		writeLn(f,'; block address: $',hexstr(org,4));
		writeLn(f);

		writeLn(f,labelname);
		for ofs:=0 to len-1 do
		begin
			if ofs mod 16=0 then
				write(f,'	.by ');
			write(f,'$',hexstr(ary[ofs],2),' ');
			if ofs mod 16=15 then
				writeLn(f);
		end;
		writeLn(f);
		inc(org,len);
	end;

	procedure generateByLines4Data(labelname:string; var ary:array of byte; len:word);
	var
		ofs,en:word;
		cnt:byte;

	begin
		writeLn(f,'; block address: $',hexstr(org,4));
		writeLn(f);

		writeLn(f,labelname);
		cnt:=0;
		for ofs:=0 to len-1 do
		begin
			for en:=0 to 63 do
			if ofs=sfxptr[en] then
			begin
				cnt:=0;
				writeLn(f);
				writeLn(f,'; address: $',hexstr(org+ofs,4),' (offset: $',hexstr(ofs,4),')');
				write(f,'data_sfx_',en);
				writeLn(f,' ; ',sfxnames[en]);
				break;
			end;
			for en:=0 to 63 do
			if ofs=tabptr[en] then
			begin
				cnt:=0;
				writeLn(f);
				write(f,'data_tab_',en);
				writeLn(f,' ; ',tabnames[en]);
				break;
			end;

			if cnt=0 then
			write(f,'	.by ');
			write(f,'$',hexstr(ary[ofs],2),' ');
			inc(cnt); if cnt=16 then
			begin
				cnt:=0;
				writeLn(f);
			end;
		end;
		writeLn(f);
		inc(org,len);
	end;

	procedure generateWOLines(labelname:string; var ary:array of word; len:word; var usage:array of shortint);
	var
		id,ofs:word;
		i:byte;

	begin
		writeLn(f,'; current block address: ',hexstr(org,4));
		writeLn(f);

		writeLn(f,labelname,'ptr');
		for id:=0 to len-1 do
		begin
			ofs:=ary[id];
			if ofs<>$ffff then
			begin
				write(f,'	dta a(data_',labelname,'_',id,') ');
				write(f,'; offset in data ',hexstr(ofs,4),' ');
			end
			else
			begin
				write(f,'	.wo $FFFF ');
				write(f,'; ',labelname,' #',id,' not defined');
			end;
			for i:=0 to 63 do
				if (usage[i]<>-1) and (usage[i]=id) then write(f,'optimized ',i,'=>',id);
			writeLn(f);
		end;
		writeLn(f);
		inc(org,len*2);
	end;

	procedure setOrigin(addr:word);
	begin
		if (addr<>0) then
		begin
			temp_org:=org; org:=addr;
		end;
	end;

	procedure restoreOrigin();
	begin
		if (temp_org<>0) then
		begin
			org:=temp_org; temp_org:=0;
		end;
	end;

begin
	if length(confFN)<>0 then
	begin
		assign(fconf,confFN);
		rewrite(fconf);
	end
	else
		fconf:=stdout;

	assign(f,fn);
	rewrite(f);
//   writeLn(f,'   opt h-');
//   writeLn(f,'   org $',hexstr(org,4));
	writeLn(fconf,'AUDIO_BUFFER_ADDR = $',hexstr(AUDIO_BUFFER_ADDR AND $FF,2),';');
	writeLn(fconf,'SFX_REGISTERS     = $',hexstr(SFX_REGISTERS,4),';');
	writeLn(fconf,'SFX_CHANNELS_ADDR = $',hexstr(SFX_CHANNELS_ADDR,4),';');
	temp_org:=0;
	NOTE_TABLE_ADDR:=NOTE_TABLE_PAGE shl 8;

	setOrigin(NOTE_TABLE_ADDR);
	writeLn(fconf,'NOTE_TABLE_PAGE   = $',hexstr(org shr 8,2),';');
	writeLn(fconf,'NOTE_TABLE_ADDR   = $',hexstr(org,4),';');
	generateByLines('note_tables',notetabs,256);
	restoreOrigin();

	setOrigin(SFX_MODE_SET_ADDR);
	writeLn(fconf,'SFX_MODE_SET_ADDR = $',hexstr(org,4),';');
	generateByLines('sfx_modes',sfxmodes,usedSFX);
	restoreOrigin();

	setOrigin(SFX_NOTE_SET_ADDR);
	writeLn(fconf,'SFX_NOTE_SET_ADDR = $',hexstr(org,4),';');
	generateByLines('sfx_note',sfxnote,usedSFX);
	restoreOrigin();

	setOrigin(SFX_TABLE_ADDR);
	writeLn(fconf,'SFX_TABLE_ADDR    = $',hexstr(org,4),';');
	generateWOLines('sfx',sfxptr,usedSFX,SFXUsage);
	restoreOrigin();

	setOrigin(TAB_TABLE_ADDR);
	writeLn(fconf,'TAB_TABLE_ADDR    = $',hexstr(org,4),';');
	generateWOLines('tab',tabptr,usedTAB,TABUsage);
	restoreOrigin();

	setOrigin(SONG_ADDR);
	writeLn(fconf,'SONG_ADDR         = $',hexstr(org,4),';');
	generateByLines('song',song,256);
	restoreOrigin();

	setOrigin(SFX_DATA_ADDR);
	writeLn(fconf,'SFX_DATA_ADDR     = $',hexstr(org,4),';');
	generateByLines4Data('data',data,topptr);

	writeLn();
//	writeLn('Data end address: ',hexstr(org-1,4));
	close(f);

	if length(confFN)<>0 then
		close(fconf);
end;

{$I ./inc/optimize.inc}
{$I ./inc/help.inc}
{$I ./inc/parseParams.inc}

begin
	writeLn('SFX Music Maker converter V1.0 by: GSD 2021');
	fillbyte(data,10240,$ff);
	fillbyte(ndata,256,$ff);
	fillbyte(sfxptr,128,$ff);
	fillbyte(tabptr,128,$ff);
	parseParams();

	loadSMM(sourceFN);

	writeLn('Origin address: ',hexstr(org,4));
	writeLn();

	if optimizeSFX then
	begin
		writeLn('SFX Optimalization...');
		SFXScanUsage();
		SFXOptimize();
		writeLn();
	end
	else
		usedSFX:=64;

	if optimizeTAB then
	begin
		writeLn('TAB Optimalization...');
		TABScanUsage();
		TABOptimize();
		writeLn();
	end
	else
		usedTAB:=64;
	saveASM(outFN);
end.
