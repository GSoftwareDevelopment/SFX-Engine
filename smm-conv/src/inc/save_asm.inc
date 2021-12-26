CONST
	COMMENT_SFX_SWITCH_ROM=' // uncomment this line if you want use RAM under ROM for data';
	COMMENT_SFX_SYNCAUDIOOUT=' // this switch turn on buffer for POKEY data. Buffer is flush after processing all channels.';
	COMMENT_SFX_PLAYBACK=' // if you want use song player, leave this definition uncomment';

procedure saveASM(fn:string; sepFiles:boolean);
var
	f,fconf,fres:text;
	oorg:word;
	dataOriginName:string;
	writeConf,
	writeRes:boolean;

	procedure _write(var f:text; s:string; isW:boolean);
	begin
		if isW then
			write(f,s);
	end;

	procedure _writeLn(var f:text; s:string; isW:boolean);
	begin
		if isW then
			writeLn(f,s);
	end;

	procedure generateBlockHeader(labelName:string; len:word);
	begin
		writeLn(f,'; block address: $',hexstr(org,4),' length: $',hexstr(len,4));
		writeLn(f);

		writeLn(f,labelname);
	end;

	procedure generateByLines(labelname:string; var ary:array of byte; len:word);
	var
		ofs:word;

	begin
		generateBlockHeader(labelName,len);
		for ofs:=0 to len-1 do
		begin
			if ofs mod 16=0 then
				write(f,'	.by ');
			write(f,'$',hexstr(ary[ofs],2),' ');
			if (ofs mod 16=15) and (ofs<len-1) then
				writeLn(f);
		end;
		writeLn(f);
		writeLn(f);
		inc(org,len);
	end;

	procedure generateByLines4Data(labelname:string; var ary:array of byte; len:word);
	var
		ofs,en:word;
		cnt,i:byte;

	begin
		generateBlockHeader(labelName,len);

		cnt:=0;
		for ofs:=0 to len-1 do
		begin
			for en:=0 to 63 do
			if ofs=sfxptr[en] then
			begin
				if cnt<>0 then writeLn(f);
				cnt:=0;
				writeLn(f);
				writeLn(f,'; address: $',hexstr(org+ofs,4),' (offset: $',hexstr(ofs,4),')');
				write(f,'data_sfx_',en);
				for i:=0 to 63 do
					if SFXUsage[i]=en then
						writeLn(f,' ; ',sfxnames[i]);
				break;
			end;
			for en:=0 to 63 do
			if ofs=tabptr[en] then
			begin
				if cnt<>0 then writeLn(f);
				cnt:=0;
				writeLn(f);
				writeLn(f,'; address: $',hexstr(org+ofs,4),' (offset: $',hexstr(ofs,4),')');
				write(f,'data_tab_',en);
				for i:=0 to 63 do
					if TABUsage[i]=en then
						writeLn(f,' ; ',tabnames[i]);
				break;
			end;

			if cnt=0 then write(f,'	.by ');
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

	procedure generateWOLines(labelname:string; var ary:array of word; len:word; var usage:array of shortint; origin:string);
	var
		id,ofs:word;
		i:byte;

	begin
		generateBlockHeader(labelName+'ptr',len);

		for id:=0 to len-1 do
		begin
			ofs:=ary[id];
			if ofs<>$ffff then
			begin
				write(f,'	dta a(');
				if length(origin)<>0 then
				begin
					write(f,origin,'.');
				end;
				write(f,'data_',labelname,'_',id);
				write(f,') ; offset in data ',hexstr(ofs,4),' ');
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

	function setOrigin(addr:word; subFn,resLabel:string):string;
	var
		o:string;

	begin
		if (addr<>0) then
		begin
			temp_org:=org; org:=addr;
			o:=concat(outPath,subFn,'.asm');
			assign(f,o);
			// write(o);
			rewrite(f);
			_writeLn(fres,resLabel+' rcasm '''+outPath+subFn+'.asm'';',writeRes);
			result:='RESOURCE.RESOURCE.RCASM0.RESOURCE._SFX_API.'+resLabel;
		end
		else
		begin
			o:=concat(outPath,fn);
			assign(f,o);
			if not fileExists(o) then
				rewrite(f)
			else
				append(f);
			// write(o);
			result:='';
		end;

		// writeLn();
		write(f,';	org $',hexstr(org,4));
		writeLn(f,'	; ',stringOfChar('-',40));
		writeLn(f);
		o:=concat(resLabel,stringOfChar(' ',17-length(resLabel)));
		if addr=0 then
			_writeLn(fconf,o+' = SFX_ORG + $'+hexstr(org-oorg,4)+';',writeConf)
		else
			_writeLn(fconf,o+' = $'+hexstr(org,4)+';',writeConf);
	end;

	procedure restoreOrigin();
	begin
		if (temp_org<>0) then
		begin
			org:=temp_org; temp_org:=0;
		end;
		close(f);
	end;

	procedure generateNoteTables();
	var
		i:byte;
		ntab1,
		ntab2,
		ntab3,
		ntab4:boolean;

	begin
		ntab1:=false; ntab2:=false; ntab3:=false; ntab4:=false;
		for i:=0 to 63 do
		begin
			if sfxnote[i]=$00 then ntab1:=true;
			if sfxnote[i]=$40 then ntab2:=true;
			if sfxnote[i]=$80 then ntab3:=true;
			if sfxnote[i]=$C0 then ntab4:=true;
		end;
		if ntab1 then generateByLines('note_table_1',notetabs[0],64);
		if ntab2 then generateByLines('note_table_2',notetabs[64],64);
		if ntab3 then generateByLines('note_table_3',notetabs[128],64);
		if ntab4 then generateByLines('note_table_4',notetabs[192],64);
	end;

	procedure defineModulators();
	var
		i:byte;
		mod_dfd,
		mod_lfd,
		mod_mfd,
		mod_hfd:boolean;

	begin
		mod_dfd:=false; mod_lfd:=false; mod_mfd:=false; mod_hfd:=false;
		for i:=0 to 63 do
		begin
			if sfxmodes[i]=$00 then mod_hfd:=true;
			if sfxmodes[i]=$01 then mod_mfd:=true;
			if sfxmodes[i]=$02 then mod_lfd:=true;
			if sfxmodes[i]=$03 then mod_dfd:=true;
		end;
		if mod_dfd then _writeLn(fconf,'{$DEFINE DFD_MOD}',writeConf);
		if mod_lfd then _writeLn(fconf,'{$DEFINE LFD_NLM_MOD}',writeConf);
		if mod_mfd then _writeLn(fconf,'{$DEFINE MFD_MOD}',writeConf);
		if mod_hfd then _writeLn(fconf,'{$DEFINE HFD_MOD}',writeConf);
	end;

begin
	temp_org:=0; oorg:=org;
	NOTE_TABLE_ADDR:=NOTE_TABLE_PAGE shl 8;

	if (length(outPath)>0) and not directoryExists(outPath) then
	begin
		write(stdout,'Creating directory `'+outPath+'`...');
		if createDir(outPath) then
			writeLn(stdout,'OK')
		else
		begin
			haltError('Failed to create directory');
		end;
	end;

	if fileExists(outPath+fn) then
	begin
		deleteFile(outPath+fn);
	end;

	writeConf:=length(confFN)<>0;
	if writeConf then
	begin
		// writeLn(outPath+confFN);
		assign(fconf,outPath+confFN);
		rewrite(fconf);
	end;

	writeRes:=length(resFN)<>0;
	if writeRes then
	begin
		// writeLn(outPath+resFn);
		assign(fres,outPath+resFN);
		rewrite(fres);
	end;

	if	(NOTE_TABLE_ADDR<$C000) and
			(SFX_MODE_SET_ADDR<$C000) and
			(SFX_NOTE_SET_ADDR<$C000) and
			(SFX_TABLE_ADDR<$C000) and
			(TAB_TABLE_ADDR<$C000) and
			(SONG_ADDR<$C000) and
			(SFX_DATA_ADDR<$C000) then _write(fconf,'// ',writeConf);
	_writeLn(fconf,'{$DEFINE SFX_SWITCH_ROM}'+COMMENT_SFX_SWITCH_ROM,writeConf);

	// writeLn(fconf,'// {$DEFINE SFX_previewChannels}');
	if AUDIO_BUFFER_ADDR=0 then _write(fconf,'// ',writeConf);
	_writeLn(fconf,'{$DEFINE SFX_SYNCAUDIOOUT}'+COMMENT_SFX_SYNCAUDIOOUT,writeConf);
	_writeLn(fconf,'{$DEFINE USE_MODULATORS}',writeConf);

	defineModulators();

	_writeLn(fconf,'{$DEFINE SFX_PLAYBACK}'+COMMENT_SFX_PLAYBACK,writeConf);
	_writeLn(fconf,'',writeConf);

	if AUDIO_BUFFER_ADDR=0 then _write(fconf,'// ',writeConf);
	_writeLn(fconf,'AUDIO_BUFFER_ADDR = $'+hexstr(AUDIO_BUFFER_ADDR,4)+';',writeConf);
	_writeLn(fconf,'SFX_REGISTERS     = $'+hexstr(SFX_REGISTERS and $ff,2)+';',writeConf);
	_writeLn(fconf,'SFX_CHANNELS_ADDR = $'+hexstr(SFX_CHANNELS_ADDR,4)+';',writeConf);
	_writeLn(fconf,'SFX_ORG           = $'+hexstr(org,4)+';',writeConf);

//	assign(f,fn);
//	rewrite(f);
//   writeLn(f,'   opt h-');
//   writeLn(f,'   org $',hexstr(org,4));
//	close(f);

	if (SFX_DATA_ADDR<>0) then
	begin
		dataOriginName:=setOrigin(SFX_DATA_ADDR,'data','SFX_DATA_ADDR');
		generateByLines4Data('data',data,topptr);
		restoreOrigin();
	end
	else
		dataOriginName:='';

	setOrigin(NOTE_TABLE_ADDR,'notetable','NOTE_TABLE_ADDR');
	_writeLn(fconf,'NOTE_TABLE_PAGE   = hi(NOTE_TABLE_ADDR);',writeConf);
	generateByLines('note_tables',notetabs,256);
//	generateNoteTables();
	restoreOrigin();

	setOrigin(SONG_ADDR,'song','SONG_ADDR');
	generateByLines('song',song,256);
	restoreOrigin();

	setOrigin(SFX_MODE_SET_ADDR,'modeset','SFX_MODE_SET_ADDR');
	generateByLines('sfx_modes',sfxmodes,usedSFX);
	restoreOrigin();

	setOrigin(SFX_NOTE_SET_ADDR,'noteset','SFX_NOTE_SET_ADDR');
	generateByLines('sfx_note',sfxnote,usedSFX);
	restoreOrigin();

	setOrigin(SFX_TABLE_ADDR,'sfxtable','SFX_TABLE_ADDR');
	generateWOLines('sfx',sfxptr,usedSFX,SFXUsage,dataOriginName);
	restoreOrigin();

	setOrigin(TAB_TABLE_ADDR,'tabtable','TAB_TABLE_ADDR');
	generateWOLines('tab',tabptr,usedTAB,TABUsage,dataOriginName);
	restoreOrigin();

	if (SFX_DATA_ADDR=0) then
	begin
		dataOriginName:=setOrigin(SFX_DATA_ADDR,'data','SFX_DATA_ADDR');
		generateByLines4Data('data',data,topptr);
		restoreOrigin();
	end;

//	writeLn('Data end address: ',hexstr(org-1,4));
//	close(f);

	if writeConf then
		close(fconf);

	if (org<>oorg) then
	begin
		_writeLn(fres,'SFX_ORG rcasm '''+outPath+fn+''';',writeRes);
	end;

	if writeRes then
		close(fres);
end;