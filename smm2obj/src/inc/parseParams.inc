procedure parseParams();
var
	currentParam:byte;
	param:string;

	function check(prm:string):boolean;
	begin
		if pos(prm,param)=1 then
		begin
			delete(param,1,length(prm));
			result:=true;
		end
		else
			result:=false;
	end;

	function getAddr(msg:string; var _val:word; def:word; errmsg:string):boolean;
	var	err:word;

	begin
		if length(msg)>0 then write(msg);
		val(param,_val,err);
		result:=err=0;
		if not result then
		begin
			WriteLn(stderr,errmsg);
			_val:=def;
		end;
		if length(msg)>0 then writeLn(hexstr(_val,4));
	end;

	procedure getAddrB(msg:string; var _val:word; def:word; errmsg:string);
	begin
		if getAddr('',_val,def,errmsg) then
		begin
			if (_val>255) then
				_val:=_val and $FF;
			writeLn(msg,' $',hexstr(_val,2));
		end;
	end;

	procedure getAddrP(msg:string; var _val:word; def:word; errmsg:string);
	begin
		if getAddr('',_val,def,errmsg) then
		begin
			if (_val<256) then
				_val:=_val shl 8
			else
				_val:=_val and $FF00;
			writeLn(msg,' $',hexstr(_val,4),' (page align)')
		end;
	end;

begin
	if paramCount<1 then
		showInstruction();
	sourceFN:=paramStr(1);
	outFN:=paramStr(2);
	currentParam:=3;
	while currentParam<=paramCount do
	begin
		param:=lowercase(paramStr(currentParam));
		if param[1]<>'-' then haltError('Invalid parameter');
		inc(currentParam);

		if check('-org:') or check('-o') then
		begin
			getAddr('Origin set: ',org,$A000,'Warning: The starting address was not specified or it''s wrong. Default origin is set =$A000');
			continue;
		end;

		if check('-audiobuffer:') or check('-Aa:') then
		begin
			getAddr('Audio buffer set:',AUDIO_BUFFER_ADDR,$E8,'ERR');
			continue;
		end;
		if check('-regs:') or check('-Ar:') then
		begin
			getAddrB('SFX registers set:',SFX_REGISTERS,$F0,'ERR');
			continue;
		end;
		if check('-chnregs:') or check('-Ac:') then
		begin
			getAddr('SFX channels registers set:',SFX_CHANNELS_ADDR,$6C0,'ERR');
			continue;
		end;
		if check('-notetable:') or check('-An:') then
		begin
			getAddrP('Note table data set:',NOTE_TABLE_ADDR,0,'ERR');
			NOTE_TABLE_PAGE:=NOTE_TABLE_ADDR shr 8;
			continue;
		end;
		if check('-sfxmodetable:') or check('-Am:') then
		begin
			getAddr('Table of SFX mode set:',SFX_MODE_SET_ADDR,0,'ERR');
			continue;
		end;
		if check('-sfxtable:') or check('-As:') then
		begin
			getAddr('Table of SFX pointers set:',SFX_TABLE_ADDR,$0,'ERR');
			continue;
		end;
		if check('-tabtable:') or check('-At:') then
		begin
			getAddr('Table of TAB pointers set:',TAB_TABLE_ADDR,$F0,'ERR');
			continue;
		end;
		if check('-song:') or check('-Ag:') then
		begin
			getAddr('Song data set:',SONG_ADDR,$F0,'ERR');
			continue;
		end;
		if check('-data:') or check('-Ad:') then
		begin
			getAddr('SFX/TAB data set:',SFX_DATA_ADDR,$F0,'ERR');
			continue;
		end;

		if check('-makeconfig')  or check('-C')then
		begin
			confFN:=CONFIG_FILENAME;
			continue;
		end;

		if check('-reduce:') or check('-R:') then
		begin
			write('Reducing: ');
			if pos('sfxonly',param)=1 then
			begin
				optimizeSFX:=true;
				writeLn('only SFXs');
			end
			else if pos('tabonly',param)=1 then
			begin
				optimizeTAB:=true;
				writeLn('only TABs');
			end
			else if pos('all',param)=1 then
			begin
				optimizeSFX:=true;
				optimizeTAB:=true;
				writeLn('all');
			end
			else
			begin
				haltError('wrong optimize parameter');
			end;
			continue;
		end;

		if check('-reindex:') or check('-I:') then
		begin
			write('Reindex: ');
			if pos('sfxonly',param)=1 then
			begin
				SFXreindex:=true;
				writeLn('only SFXs');
			end
			else if pos('tabonly',param)=1 then
			begin
				TABreindex:=true;
				writeLn('only TABs');
			end
			else if pos('all',param)=1 then
			begin
				SFXreindex:=true;
				TABreindex:=true;
				writeLn('all');
			end
			else
			begin
				haltError('wrong reindex parameter');
			end;
			continue;
		end;
		writeLn('What is this `',param,'`? :O');
		haltError('');
	end;
end;
