procedure parseParams();
var
	currentParam:byte;
	param:string;
  fn:string;

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

	function getAddr(msg:string; var _val:word; def:word; var fileName:string; canFileSpec:boolean = true):boolean;
	var	err,fnDelimPos:word;

	begin
		fnDelimPos:=0;
		if canFileSpec then
		begin
			fnDelimPos:=pos(':',param);
			if fnDelimPos>0 then
			begin
				fileName:=copy(param,fnDelimPos+1);
				param:=copy(param,1,fnDelimPos-1); // extract address from parameter
				// writeLn(stdout,fn,' ',param);
			end;
		end
		else
		begin
			fileName:='';
		end;
		val(param,_val,err);
		result:=err=0;
		if not result then
		begin
			if _val>0 then
				WriteLn(stderr,format(ERR_BAD_ADDRESS_DEFAULT_SET,[msg,def]))
			else
				WriteLn(stderr,format(ERR_BAD_ADDRESS_ORIGIN_CONTINUE,[msg]));
			_val:=def;
		end
		else
			if length(msg)>0 then
				if canFileSpec and (length(fileName)>0) then
					if fnDelimPos>0 then
						writeLnStdOut(STDOUT_ADDRESS_SET+STDOUT_OUTPUT_FILENAME_SET,[msg,4,_val,fileName])
					else
						writeLnStdOut(STDOUT_ADDRESS_SET+STDOUT_OUTPUT_FILENAME,[msg,4,_val,fileName])
				else
					writeLnStdOut(STDOUT_ADDRESS_SET,[msg,4,_val])
	end;

	procedure getAddrB(msg:string; var _val:word; def:word; var fileName:string; canFileSpec:boolean = true);
	begin
		if getAddr('',_val,def,fileName,canFileSpec) then
		begin
			if (_val>255) then
				_val:=_val and $FF;
			writeLnStdOut(STDOUT_ADDRESS_SET,[msg,2,_val]);
		end;
	end;

	procedure getAddrP(msg:string; var _val:word; def:word; var fileName:string; canFileSpec:boolean = true);
	begin
		if getAddr('',_val,def,fileName,canFileSpec) then
		begin
			if (_val<256) then
				_val:=_val shl 8
			else
				_val:=_val and $FF00;
			writeLnStdOut(STDOUT_ADDRESS_SET+STDOUT_PAGE_ALIGN,[msg,4,_val]);
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
				writeLnStdOut(STDOUT_INPUT_FILE,[sourceFN]);
			end
			else
			begin
				outFN:=param;
				writeLnStdOut(STDOUT_OUTPUT_FILE,[outFN]);
			end;
			continue;
		end;

		if check('version') or check('-v') then
		begin
			WriteLn(stdout,VERSION); halt;
		end;
		//
		//

		if check('org:') or check('Ao:') then
		begin
			getAddr(STDOUT_ORIGIN,org,$A000,orgFN,true);
			continue;
		end;

		if check('audiobuffer:') or check('Aa:') then
		begin
			getAddr(STDOUT_AUDIO_BUFFER,AUDIO_BUFFER_ADDR,$E8,fn,false);
			continue;
		end;
		if check('regs:') or check('Ar:') then
		begin
			getAddrB(STDOUT_SFX_REGISTERS,SFX_REGISTERS,$F0,fn,false);
			continue;
		end;
		if check('chnregs:') or check('Ac:') then
		begin
			getAddr(STDOUT_SFX_CHANNELS_REGS,SFX_CHANNELS_ADDR,$6C0,fn,false);
			continue;
		end;

		//
		//

		// TODO: Rozważ podanie nazwy pliku dla bloków danych!
		if check('notetable:') or check('An:') then
		begin
			getAddrP(STDOUT_NOTE_TABLE_DATA,NOTE_TABLE_ADDR,0,noteTabFN,true);
			NOTE_TABLE_PAGE:=NOTE_TABLE_ADDR shr 8;
			continue;
		end;
		if check('sfxnotetable:') or check('Ae:') then
		begin
			getAddr(STDOUT_TABLE_OF_SFX_NOTE,SFX_NOTE_SET_ADDR,0,sfxnotesFN,true);
			continue;
		end;
		if check('sfxmodetable:') or check('Am:') then
		begin
			getAddr(STDOUT_TABLE_OF_SFX_MODE,SFX_MODE_SET_ADDR,0,sfxmodesFN,true);
			continue;
		end;
		if check('sfxtable:') or check('As:') then
		begin
			getAddr(STDOUT_TABLE_OF_SFX_PTRS,SFX_TABLE_ADDR,$0,sfxTabFN,true);
			continue;
		end;
		if check('tabtable:') or check('At:') then
		begin
			getAddr(STDOUT_TABLE_OF_TAB_PTRS,TAB_TABLE_ADDR,$0,tabTabFN,true);
			continue;
		end;
		if check('songdata:') or check('Ag:') then
		begin
			getAddr(STDOUT_SONG_DATA,SONG_ADDR,$0,songTabFN,true);
			continue;
		end;
		if check('data:') or check('Ad:') then
		begin
			getAddr(STDOUT_SFXTAB_DATA,SFX_DATA_ADDR,$0,dataFN,true);
			continue;
		end;

		//
		//

		if check('makeconfig') or check('MC') then
		begin
			If check(':') Then
				confFN := param
			Else
				confFN := DEFAULT_CONFIG_FILENAME;
			continue;
		end;
		if check('makeresource') or check('MR') then
		begin
			If check(':') Then
				resFn := param
			Else
				resFN := DEFAULT_RESOURCE_FILENAME;
			continue;
		end;

		// TODO: rozważ przełącznik ustalający, czy i jak generować nagłówki dla plików MADASM

		//
		//

		if check('reduce:') or check('R:') then
		begin
			if pos('sfxonly',param)=1 then
			begin
				SFXreduce:=true;
				param:=STDOUT_SFX_ONLY;
			end
			else if pos('tabonly',param)=1 then
			begin
				TABreduce:=true;
				param:=STDOUT_TAB_ONLY;
			end
			else if pos('all',param)=1 then
			begin
				SFXreduce:=true;
				TABreduce:=true;
				param:=STDOUT_ALL;
			end
			else
				haltError('wrong reducing parameter');
			writeLnStdOut(STDOUT_REDUCE_SET,[param]);
			continue;
		end;

		if check('reindex:') or check('I:') then
		begin
			if pos('sfxonly',param)=1 then
			begin
				SFXreindex:=true;
				param:=STDOUT_SFX_ONLY;
			end
			else if pos('tabonly',param)=1 then
			begin
				TABreindex:=true;
				param:=STDOUT_TAB_ONLY;
			end
			else if pos('all',param)=1 then
			begin
				SFXreindex:=true;
				TABreindex:=true;
				param:=STDOUT_ALL;
			end
			else
				haltError('wrong reindex parameter');

			writeLnStdOut(STDOUT_REINDEX_SET,[param]);
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

		writeLn(stderr,format('What is this %s? :O',[param]));
		haltError('Unrecognized parameter');
	end;

	if length(outFN)>0 then
	begin
		// set output path from primaary file name
		outPath:=extractFilePath(outFN);
		outFN:=extractFileName(outFN);
	end
	else
	begin
		// set output path form source file name;
		outPath:=extractFilePath(sourceFN);
		outFN:=extractFileName(sourceFN)+'.asm';
	end;

	if length(orgFN)>0 then
	begin
		// if origin have specified file name,
		// primary file is set to thin file
		outFN:=orgFN;
	end;
end;
