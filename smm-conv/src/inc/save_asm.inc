{$I const/save_asm.inc}

function createFile(var f:text; fn:string; allowAppend:boolean = false):boolean;
begin
	result:=not fileExists(fn);
	assign(f,fn);
	if allowAppend and not result then
		append(f)
	else
	begin
		rewrite(f);
		writeLnStdOut(STDOUT_CREATE_FILE,[fn]);
	end;
end;

procedure writeDEFINE(const lab:string; const comment:string = '');
begin
	_writeLn(fconf,format(DEFINE_ENTRY,[lab,comment]),writeConf);
end;

procedure writeConfConst(const _type:byte; const prm:array of const);
var
	CONST_TYPE:string;

begin
	case _type of
		1: CONST_TYPE:=CONF_CONST_SET_ORG_RELATIVE;
		2: CONST_TYPE:=CONF_CONST_SET_PAGE;
	else
		CONST_TYPE:=CONF_CONST_SET_ABSOLUTE;
	end;
	_writeLn(fconf,format(CONST_TYPE,prm),writeConf);
end;

procedure writeResource(const lab:string; path:string);
begin
	_writeLn(fres,format(RESOURCE_ENTRY,[lab,path]),writeRes);
end;

//
//
//

procedure saveASM(fn:string);
var
	oorg:word;
	dataOriginName:string;
	i:byte;
  sizeAry:array[0..63] of word;

	procedure generateBlockHeader(labelName:string; len:word);
	begin
		_EOL(F);
		writeLn(f,format(INDENT+ASM_COMMENT+COMMENT_BLOCK_INFO,[org,len]));
		writeLn(f,labelname);
	end;

	procedure generateByLines(labelname:string; var ary:array of byte; len:word);
	var
		ofs:word;
		out:string = '';

	begin
		generateBlockHeader(labelName,len);
		if len=0 then exit;
		dec(len);
		for ofs:=0 to len do
		begin
			if ofs mod 16=0 then
				out:=INDENT+ASM_DTA;
			out:=out+format(ASM_DTA_BYTE,[ary[ofs]]);
			if ((ofs mod 16=15) and (ofs<len)) or (ofs=len) then
				writeLn(f,out)
			else
				out:=out+ASM_DTA_SEPARATOR;
		end;
		_EOL(f,2);
		inc(org,len+1);
	end;

	procedure generateByLines4Data(labelName:string);
	var
		i,len:word;

	begin
		len:=SMM.getSFXDataSize+SMM.getTABDataSize;
		generateBlockHeader(labelName,len);

		for i:=0 to 63 do
			if (SMM.SFX[i]<>nil) then
				generateByLines(
					format(LABEL_DATA_SFX_N,[i]),
					SMM.SFX[i]^.data^,
					SMM.SFX[i]^.size
				);

		for i:=0 to 63 do
			if (SMM.TAB[i]<>nil) then
				generateByLines(
					format(LABEL_DATA_TAB_N,[i]),
					SMM.TAB[i]^.data^,
					SMM.TAB[i]^.size
				);

		_EOL(F);
		inc(org,len);
	end;

	procedure generateWOLines(labelName:string; const flab:string; var ary:array of word; len:word; origin:string);
	var
		id,ofs:word;
		lab:string;

	begin
		generateBlockHeader(labelName,len*2);
		if len=0 then exit;
		ofs:=0;
		for id:=0 to len-1 do
		begin
			if ary[id]<>$ffff then
			begin
				if length(origin)<>0 then
					lab:=origin+'.'
				else
					lab:='';
				lab:=lab+format(flab,[id]);
				write(f,format(INDENT+ASM_DTA+ASM_DTA_ADDRESS+INDENT+ASM_COMMENT+COMMENT_DATA_OFFSET,[lab,ofs]));
				inc(ofs,ary[id]);
			end
			else
				write(f,format(INDENT+ASM_DTA+ASM_DTA_ADDRESS+INDENT+ASM_COMMENT+COMMENT_DATA_NOT_DEF,['$FFFF',labelName,id]));

			// for i:=0 to 63 do
			// 	if (usage[i]<>-1) and (usage[i]=id) then
			// 		write(f,format(INDENT+ASM_COMMENT+COMMENT_DATA_REINDEXED,[i,id]));
			_EOL(f);
		end;
		_EOL(f);
		inc(org,len*2);
	end;

	function setOrigin(addr:word; subFn,resLabel:string):string;
	var
		o:string;

	begin
		if (addr=0) then
		begin
			o:=concat(outPath,fn);
			// create or continue primary file (origin file)
			createFile(f,o,true);
			result:='';
		end
		else
		begin
			temp_org:=org; org:=addr;
			o:=concat(outPath,subFn);

			// create new file for block
			createFile(f,o);
			writeResource(resLabel,outPath+subFn);
			result:=format(RESOURCE_PATH,[resLabel]);
		end;

		// TODO: ustal za pomocą przełącznika, czy i jakie generować nagłówki dla plików MADASM
		writeLn(f,format(INDENT+ASM_COMMENT+ASM_ORG,[org]));

		// create entry in config file
		if addr=0 then
			writeConfConst(CONST_ORG_RELATIVE,[resLabel,org-oorg])
		else
			writeConfConst(CONST_ABSOLUTE,[resLabel,4,org]);
	end;

	procedure restoreOrigin();
	begin
		if (temp_org<>0) then
		begin
			org:=temp_org; temp_org:=0;
		end;
		close(f);
	end;

	procedure defineModulators();
	var
		i:byte;
		mod_dfd,
		mod_lfd,
		mod_mfd,
		mod_hfd:boolean;
		SFX:TSFXData;

	begin
		mod_dfd:=false; mod_lfd:=false; mod_mfd:=false; mod_hfd:=false;
		for i:=0 to 63 do
		begin
			if SMM.SFX[i]=nil then continue;

			SFX:=SMM.SFX[i]^;
			if sfx.mode=$00 then mod_hfd:=true;
			if sfx.mode=$01 then mod_mfd:=true;
			if sfx.mode=$02 then mod_lfd:=true;
			if sfx.mode=$03 then mod_dfd:=true;
		end;
		if mod_dfd then writeDEFINE(DEFINE_MOD_DFD);
		if mod_lfd then writeDEFINE(DEFINE_MOD_LFD_NLM);
		if mod_mfd then writeDEFINE(DEFINE_MOD_MFD);
		if mod_hfd then writeDEFINE(DEFINE_MOD_HFD);
	end;

begin
	temp_org:=0; oorg:=org;
	NOTE_TABLE_ADDR:=NOTE_TABLE_PAGE shl 8;

	if (length(outPath)>0) and not directoryExists(outPath) then
	begin
		writeStdOut(STDOUT_CREATE_DIRECTORY,[outPath]);
		if createDir(outPath) then
			writeLnStdOut(STDOUT_CREATE_DIRECTORY_OK,[])
		else
			haltError(ERR_FAILED_CREATE_DIR);
	end;

	if fileExists(outPath+fn) then
		// Delete primary output file, if exist!
		deleteFile(outPath+fn);

	writeConf:=length(confFN)<>0;
	if writeConf then
		createFile(fconf,outPath+confFN);

	writeRes:=length(resFN)<>0;
	if writeRes then
		createFile(fres,outPath+resFN);

	if	(NOTE_TABLE_ADDR<$C000) and
			(SFX_MODE_SET_ADDR<$C000) and
			(SFX_NOTE_SET_ADDR<$C000) and
			(SFX_TABLE_ADDR<$C000) and
			(TAB_TABLE_ADDR<$C000) and
			(SONG_ADDR<$C000) and
			(SFX_DATA_ADDR<$C000) then _write(fconf,COMMENT,writeConf);
	writeDEFINE(DEFINE_SFX_SWITCH_ROM,
	            INDENT+COMMENT+COMMENT_SFX_SWITCH_ROM);

	// writeLn(fconf,'// {$DEFINE SFX_previewChannels}');

	if AUDIO_BUFFER_ADDR=0 then _write(fconf,COMMENT,writeConf);
	writeDEFINE(DEFINE_SFX_SYNC_AUDIO_OUT,
	            INDENT+COMMENT+COMMENT_SFX_SYNCAUDIOOUT);

	writeDEFINE(DEFINE_USE_MODULATORS);
	defineModulators();

	writeDEFINE(DEFINE_SFX_PLAYBACK,
	            INDENT+COMMENT+COMMENT_SFX_PLAYBACK);
	if writeConf then _EOL(fconf);

	if AUDIO_BUFFER_ADDR=0 then _write(fconf,COMMENT,writeConf);
	writeConfConst(CONST_ABSOLUTE,[CONST_AUDIO_BUFFER_ADDR,4,AUDIO_BUFFER_ADDR]);
	writeConfConst(CONST_ABSOLUTE,[CONST_SFX_REGISTERS,2,SFX_REGISTERS and $FF]);
	writeConfConst(CONST_ABSOLUTE,[CONST_SFX_CHANNELS_ADDR,4,SFX_CHANNELS_ADDR]);
	writeConfConst(CONST_ABSOLUTE,[CONST_SFX_ORG,4,org]);


// TODO: ustal za pomocą przełącznika, czy i jakie generować nagłówki dla plików MADASM
//	assign(f,fn);
//	rewrite(f);
//   writeLn(f,'   opt h-');
//   writeLn(f,'   org $',hexstr(org,4));
//	close(f);

	if (SFX_DATA_ADDR<>0) then
	begin
		dataOriginName:=setOrigin(SFX_DATA_ADDR,
		                          dataFN,
															CONST_SFX_DATA_ADDR);
		generateByLines4Data(LABEL_DATA);
		restoreOrigin();
	end
	else
		dataOriginName:='';

	setOrigin(NOTE_TABLE_ADDR,
	          noteTabFN,
						CONST_NOTE_TABLE_ADDR);
	writeConfConst(CONST_PAGE,[CONST_NOTE_TABLE_PAGE,CONST_NOTE_TABLE_ADDR]);
	for i:=0 to 3 do
		if SMM.noteTable[i]<>nil then
			generateByLines(format(LABEL_NOTE_TABLE_n,[i]),SMM.noteTable[i]^,64)
		else
		begin
			generateBlockHeader(format(LABEL_NOTE_TABLE_n,[i]),64);
			writeLn(f,'	64:dta $00;');
		end;
	restoreOrigin();

	setOrigin(SONG_ADDR,
	          songTabFN,
						CONST_SONG_ADDR);
	generateByLines(LABEL_SONG_DATA,SMM.song,256);
	restoreOrigin();

	setOrigin(SFX_MODE_SET_ADDR,
	          sfxmodesFN,
						CONST_SFX_MODE_SET_ADDR);
	generateByLines(LABEL_SFX_MODES_TABLE,SMM.SFXModTable,usedSFX);
	restoreOrigin();

	setOrigin(SFX_NOTE_SET_ADDR,
	          sfxnotesFN,
						CONST_SFX_NOTE_SET_ADDR);
	generateByLines(LABEL_SFX_NOTES_TABLE,SMM.SFXNoteTable,usedSFX);
	restoreOrigin();

	setOrigin(SFX_TABLE_ADDR,
	          sfxTabFN,
						CONST_SFX_TABLE_ADDR);
  for i:=0 to 63 do
    if SMM.SFX[i]<>nil then
      sizeAry[i]:=SMM.SFX[i]^.size
    else
      sizeAry[i]:=$FFFF;
  generateWOLines(LABEL_SFX_TABLE,LABEL_DATA_SFX_N,sizeAry,usedSFX,dataOriginName);
	restoreOrigin();

	setOrigin(TAB_TABLE_ADDR,
	          tabTabFN,
						CONST_TAB_TABLE_ADDR);
  for i:=0 to 63 do
    if SMM.TAB[i]<>nil then
      sizeAry[i]:=SMM.TAB[i]^.size
    else
      sizeAry[i]:=$FFFF;
	generateWOLines(LABEL_TAB_TABLE,LABEL_DATA_TAB_N,sizeAry,usedTAB,dataOriginName);
	restoreOrigin();

	if (SFX_DATA_ADDR=0) then
	begin
		dataOriginName:=setOrigin(SFX_DATA_ADDR,
		                          dataFN,
															CONST_SFX_DATA_ADDR);
		generateByLines4Data(LABEL_DATA);
		restoreOrigin();
	end;

//	writeLn('Data end address: ',hexstr(org-1,4));
//	close(f);

	if writeConf then
		close(fconf);

	if (org<>oorg) then
		writeResource(CONST_SFX_ORG,outPath+fn);

	if writeRes then
		close(fres);
end;
