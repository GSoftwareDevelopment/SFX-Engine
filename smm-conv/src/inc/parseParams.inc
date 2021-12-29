CONST
	ERR_BAD_ADDRESS = 'Warning: address was not specified or it''s wrong.'+EOL;

	MSG_ORIGIN = 'Origin';
	MSG_AUDIO_BUFFER = 'Audio buffer';
	MSG_SFX_REGISTERS = 'SFX registers';
	MSG_SFX_CHANNELS_REGS ='SFX channels registers';
	MSG_NOTE_TABLE_DATA = 'Note table data';
	MSG_TABLE_OF_SFX_NOTE = 'Table of SFX note';
	MSG_TABLE_OF_SFX_MODE = 'Table of SFX mode';
	MSG_TABLE_OF_SFX_PTRS = 'Table of SFX pointers';
	MSG_TABLE_OF_TAB_PTRS = 'Table of TAB pointers';
	MSG_SONG_DATA = 'Song data';
	MSG_SFXTAB_DATA = 'SFX/TAB data';
	MSG_ORIGIN_CONTINUE = ' continue origin';
	MSG_SET_TO_DEFAULT = ' is set to default ';

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
		if length(msg)>0 then
			if verbose>0 then write(stdout,msg);
		val(param,_val,err);
		result:=err=0;
		if not result then
		begin
			WriteLn(stderr,errmsg);
			_val:=def;
		end;
		if length(msg)>0 then
			if verbose>0 then writeLn(stdout,hexstr(_val,4));
	end;

	procedure getAddrB(msg:string; var _val:word; def:word; errmsg:string);
	begin
		if getAddr('',_val,def,errmsg) then
		begin
			if (_val>255) then
				_val:=_val and $FF;
			if verbose>0 then writeLn(stdout,msg,' $',hexstr(_val,2));
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
			if verbose>0 then writeLn(stdout,msg,' $',hexstr(_val,4),' (page align)')
		end;
	end;

begin
	if paramCount=0 then
		showInstruction();
	currentParam:=1;
	while currentParam<=paramCount do
	begin
		param:=paramStr(currentParam);
//		if param[1]<>'-' then haltError('Invalid parameter');
		inc(currentParam);

		if not check('-') then
		begin
			if length(sourceFN)=0 then
			begin
				sourceFN:=param;
				writeLn(stdout,'Music file: '+sourceFN);
			end
			else
			begin
				outFN:=param;
				writeLn(stdout,'Out file: '+outFN);
			end;
			continue;
		end;

		if check('version') or check('-v') then
		begin
			WriteLn(VERSION); halt;
		end;
		//
		//

		if check('org:') or check('Ao:') then
		begin
			getAddr(MSG_ORIGIN+' set: ',org,$A000,ERR_BAD_ADDRESS+MSG_ORIGIN+MSG_SET_TO_DEFAULT+'($A000)');
			continue;
		end;

		if check('audiobuffer:') or check('Aa:') then
		begin
			getAddr(MSG_AUDIO_BUFFER+' set:',AUDIO_BUFFER_ADDR,$E8,ERR_BAD_ADDRESS+MSG_SFX_CHANNELS_REGS+MSG_SET_TO_DEFAULT+'($00E8)');
			continue;
		end;
		if check('regs:') or check('Ar:') then
		begin
			getAddrB(MSG_SFX_REGISTERS+' set:',SFX_REGISTERS,$F0,ERR_BAD_ADDRESS+MSG_SFX_CHANNELS_REGS+MSG_SET_TO_DEFAULT+'($00F0)');
			continue;
		end;
		if check('chnregs:') or check('Ac:') then
		begin
			getAddr(MSG_SFX_CHANNELS_REGS+' set:',SFX_CHANNELS_ADDR,$6C0,ERR_BAD_ADDRESS+MSG_SFX_CHANNELS_REGS+MSG_SET_TO_DEFAULT+'($06C0)');
			continue;
		end;

		//
		//

		// TODO: Rozważ podanie nazwy pliku dla bloków danych!
		if check('notetable:') or check('An:') then
		begin
			getAddrP(MSG_NOTE_TABLE_DATA+' set:',NOTE_TABLE_ADDR,0,ERR_BAD_ADDRESS+MSG_NOTE_TABLE_DATA+MSG_ORIGIN_CONTINUE);
			NOTE_TABLE_PAGE:=NOTE_TABLE_ADDR shr 8;
			continue;
		end;
		if check('sfxnotetable:') or check('Ae:') then
		begin
			getAddr(MSG_TABLE_OF_SFX_NOTE+' set:',SFX_NOTE_SET_ADDR,0,ERR_BAD_ADDRESS+MSG_TABLE_OF_SFX_NOTE+MSG_ORIGIN_CONTINUE);
			continue;
		end;
		if check('sfxmodetable:') or check('Am:') then
		begin
			getAddr(MSG_TABLE_OF_SFX_MODE+' set:',SFX_MODE_SET_ADDR,0,ERR_BAD_ADDRESS+MSG_TABLE_OF_SFX_MODE+MSG_ORIGIN_CONTINUE);
			continue;
		end;
		if check('sfxtable:') or check('As:') then
		begin
			getAddr(MSG_TABLE_OF_SFX_PTRS+' set:',SFX_TABLE_ADDR,$0,ERR_BAD_ADDRESS+MSG_TABLE_OF_SFX_PTRS+MSG_ORIGIN_CONTINUE);
			continue;
		end;
		if check('tabtable:') or check('At:') then
		begin
			getAddr(MSG_TABLE_OF_TAB_PTRS+' set:',TAB_TABLE_ADDR,$F0,ERR_BAD_ADDRESS+MSG_TABLE_OF_TAB_PTRS+MSG_ORIGIN_CONTINUE);
			continue;
		end;
		if check('songdata:') or check('Ag:') then
		begin
			getAddr(MSG_SONG_DATA+' set:',SONG_ADDR,$F0,ERR_BAD_ADDRESS+MSG_SONG_DATA+MSG_ORIGIN_CONTINUE);
			continue;
		end;
		if check('data:') or check('Ad:') then
		begin
			getAddr(MSG_SFXTAB_DATA+' set:',SFX_DATA_ADDR,$F0,ERR_BAD_ADDRESS+MSG_SFXTAB_DATA+MSG_ORIGIN_CONTINUE);
			continue;
		end;

		//
		//

		if check('makeconfig') or check('MC') then
		begin
			confFN:=CONFIG_FILENAME;
			continue;
		end;
		if check('makeresource') or check('MR') then
		begin
			If check(':') Then
				confFn := param
			Else
				confFN := CONFIG_FILENAME;
			resFN:=RESOURCE_FILENAME;
			continue;
		end;

		// TODO: rozważ przełącznik ustalający, czy i jak generować nagłówki dla plików MADASM

		//
		//

		if check('reduce:') or check('R:') then
		begin
			if verbose>0 then write(stdout,'Reducing: ');
			if pos('sfxonly',param)=1 then
			begin
				optimizeSFX:=true;
				if verbose>0 then writeLn(stdout,'only SFXs');
			end
			else if pos('tabonly',param)=1 then
			begin
				optimizeTAB:=true;
				if verbose>0 then writeLn(stdout,'only TABs');
			end
			else if pos('all',param)=1 then
			begin
				optimizeSFX:=true;
				optimizeTAB:=true;
				if verbose>0 then writeLn(stdout,'all');
			end
			else
			begin
				haltError('wrong reducing parameter');
			end;
			continue;
		end;

		if check('reindex:') or check('I:') then
		begin
			if verbose>0 then write(stdout,'Reindex: ');
			if pos('sfxonly',param)=1 then
			begin
				SFXreindex:=true;
				if verbose>0 then writeLn(stdout,'only SFXs');
			end
			else if pos('tabonly',param)=1 then
			begin
				TABreindex:=true;
				if verbose>0 then writeLn(stdout,'only TABs');
			end
			else if pos('all',param)=1 then
			begin
				SFXreindex:=true;
				TABreindex:=true;
				if verbose>0 then writeLn(stdout,'all');
			end
			else
			begin
				haltError('wrong reindex parameter');
			end;
			continue;
		end;

		if check('verbose') or check('v') then
		begin
			verbose:=2; continue;
		end;
		if check('silent') or check('s') then
		begin
			verbose:=0; continue;
		end;

		writeLn(stderr,'What is this `',param,'`? :O');
		haltError('Unrecognized parameter');
	end;

	if length(outFN)>0 then
	begin
		outPath:=extractFilePath(outFN);
		outFN:=extractFileName(outFN);
	end
	else
	begin
		outPath:=extractFilePath(sourceFN);
		outFN:=extractFileName(sourceFN)+'.asm';
	end;
end;
