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
		writeLn(f,'; block address: $',hexstr(org,4));
		writeLn(f);

		writeLn(f,labelname);
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
		write(f,';	org $',hexstr(org,4));
		writeLn(f,'	; ',stringOfChar('-',40));
		writeLn(f);
	end;

	procedure restoreOrigin();
	begin
		if (temp_org<>0) then
		begin
			org:=temp_org; temp_org:=0;
		end;
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
		if mod_dfd then writeLn(fconf,'{$DEFINE DFD_MOD}');
		if mod_lfd then writeLn(fconf,'{$DEFINE LFD_NLM_MOD}');
		if mod_mfd then writeLn(fconf,'{$DEFINE MFD_MOD}');
		if mod_hfd then writeLn(fconf,'{$DEFINE HFD_MOD}');
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
	writeLn(fconf,'// {$DEFINE SFX_SWITCH_ROM}');
	writeLn(fconf,'// {$DEFINE SFX_previewChannels}');
	writeLn(fconf,'{$DEFINE SFX_SYNCAUDIOOUT}');
	writeLn(fconf,'{$DEFINE USE_MODULATORS}');

	defineModulators();

	writeLn(fconf,'// {$DEFINE SFX_PLAYBACK}');
	writeLn(fconf);

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

	setOrigin(SONG_ADDR);
	writeLn(fconf,'SONG_ADDR         = $',hexstr(org,4),';');
	generateByLines('song',song,256);
	restoreOrigin();

	setOrigin(SFX_TABLE_ADDR);
	writeLn(fconf,'SFX_TABLE_ADDR    = $',hexstr(org,4),';');
	generateWOLines('sfx',sfxptr,usedSFX,SFXUsage);
	restoreOrigin();

	setOrigin(TAB_TABLE_ADDR);
	writeLn(fconf,'TAB_TABLE_ADDR    = $',hexstr(org,4),';');
	generateWOLines('tab',tabptr,usedTAB,TABUsage);
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