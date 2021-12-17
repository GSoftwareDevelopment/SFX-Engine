procedure SFXScanUsage();
var
	nSFX:byte;

	function checkSFXinTABs(SFXCheck:byte):boolean;
	var
		nTAB,tabrow,tablen:byte;
		tabofs:word;
		rd2:byte;

	begin
		result:=false;
		for nTAB:=0 to 63 do
		begin
			if tabptr[nTAB]=$FFFF then continue;
			tabofs:=tabptr[nTAB]; tablen:=tabSizes[nTAB] div 2;
			for tabrow:=0 to tablen-1 do
			begin
				inc(tabofs);
				rd2:=data[tabofs]; inc(tabofs);
				if rd2 and $80<>0 then continue;
				if rd2 and $3f=SFXCheck then result:=true;
			end;
		end;
	end;

begin
	Write('SFX used: '); usedSFX:=0;
	for nSFX:=0 to 63 do
	begin
		if checkSFXinTABs(nSFX) then
		begin
			SFXUsage[nSFX]:=nSFX;
			Write('#',nSFX,' ');
			inc(usedSFX);
		end
		else
		begin
			SFXUsage[nSFX]:=-1;
		end;
	end;
	WriteLn();
	WriteLn('Summary: ',usedSFX,' SFX(s) is used.');
end;

procedure SFXOptimize();
var
	nSFX,newSFX:byte;
	nTAB:Byte;

	procedure moveSFX(orgSFX,newSFX:byte);
	var
		sfxofs,sfxsize:word;
		sfxmod,sfxnt:byte;

	begin
		// WriteLn('move SFX #',orgSFX,' to #',newSFX);
		sfxofs:=sfxptr[orgSFX]; sfxsize:=sfxSizes[orgSFX];
		sfxmod:=sfxmodes[orgSFX]; sfxnt:=sfxnote[orgSFX];

		move(data[sfxofs],ndata,sfxsize); // copy entry to temporary buffer
		move(data[sfxofs+sfxsize],data[sfxofs],topptr-sfxsize); dec(topptr,sfxsize); // delete old data
		sfxptr[orgSFX]:=$ffff; sfxSizes[orgSFX]:=0;
		sfxmodes[orgSFX]:=$00; sfxnote[orgSFX]:=$00;

		move(data[ntopptr],data[ntopptr+sfxsize],topptr); inc(topptr,sfxsize); // make place for entry
		move(ndata,data[ntopptr],sfxsize); // copy entry

		sfxptr[newSFX]:=ntopptr; sfxSizes[newSFX]:=sfxsize;
		sfxmodes[newSFX]:=sfxmod; sfxnote[newSFX]:=sfxnt;
		inc(ntopptr,sfxsize);

	//      move(data[sfxofs],ndata[ntopptr],sfxsize);
	end;

	procedure changeSFXinTAB(nTAB:byte);
	var
		tabrow,tablen:byte;
		tabofs:word;
		rd1,rd2:byte;
		oSFX:byte;
		nSFX:shortint;

	begin
		if tabptr[nTAB]=$FFFF then exit;
		tabofs:=tabptr[nTAB]; tablen:=tabSizes[nTAB] div 2;
		tabptr[nTAB]:=ntopptr;
		for tabrow:=0 to tablen-1 do
		begin
			rd1:=data[tabofs]; inc(tabofs);
			rd2:=data[tabofs]; inc(tabofs);
			if rd2 and $80=0 then
			begin
				oSFX:=rd2 and $3f;
				nSFX:=SFXUsage[oSFX];
				if nSFX=-1 then HaltError('Something goes wrong.');
				rd2:=(rd2 and $c0) or nSFX;
			end;
			data[ntopptr]:=rd1; inc(ntopptr);
			data[ntopptr]:=rd2; inc(ntopptr);
		end;
	end;

begin
	newSFX:=0; ntopptr:=0;
	for nSFX:=0 to 63 do
	begin
		if SFXUsage[nSFX]<>-1 then
		begin
			if not SFXreindex then newSFX:=SFXUsage[nSFX];
			moveSFX(nSFX,newSFX);
			if SFXreindex then
			begin
				SFXUsage[nSFX]:=newSFX;
				inc(newSFX);
			end;
		end
		else
		begin
			sfxptr[nSFX]:=$ffff; sfxSizes[nSFX]:=0;
			sfxmodes[nSFX]:=$00; sfxnote[nSFX]:=$00;
		end;
	end;
	for nTAB:=0 to 63 do
	begin
		changeSFXinTAB(nTAB);
	end;
	(* move(ndata,data,10240); *) topptr:=ntopptr;
end;

procedure TABScanUsage();
var
	nTAB:byte;

	function checkTABinSONG(nTAB:byte):boolean;
	var
		songofs:word;
		se,track:byte;

	begin
		result:=false;
		songofs:=0;
		repeat
			for track:=0 to 3 do
			begin
				se:=song[songofs];
				if (track=0) and (se and $80<>0) then
				begin
					inc(songofs,4); break;
				end;
				inc(songofs);

				if se and $40=0 then
				begin
					if se and $3f=nTAB then result:=true;
				end;
			end;
		until songofs=$100;
	end;

begin
	Write('TAB used: '); usedTAB:=0;
	for nTAB:=0 to 63 do
	begin
		if checkTABinSONG(nTAB) then
		begin
			Write('#',nTAB,' ');
			TABUsage[nTAB]:=nTAB;
			inc(usedTAB);
		end
		else
		begin
			TABUsage[nTAB]:=-1;
		end;
	end;
	WriteLn();
	WriteLn('Summary: ',usedTAB,' TAB(s) is used.');
end;

procedure TABOptimize();
var
	nSFX,nTAB,newTAB:byte;
	songofs:word;
	se,track:byte;

	procedure moveTAB(oldTAB,newTAB:byte);
	var
		tabofs,tabsize:word;

	begin
		// WriteLn('move TAB #',oldTAB,' to #',newTAB);
		tabofs:=tabptr[oldTAB]; tabsize:=tabSizes[oldTAB];

		move(data[tabofs],ndata,tabsize); // copy TAB data to temporary array

		move(data[tabofs+tabsize],data[tabofs],topptr-tabsize); dec(topptr,tabsize); // delete old data
		tabptr[oldTAB]:=$FFFF; tabSizes[oldTAB]:=0;

		move(data[ntopptr],data[ntopptr+tabsize],topptr); inc(topptr,tabsize); // make place for entry
		move(ndata,data[ntopptr],tabsize); // copy TAB entry
		tabptr[newTAB]:=ntopptr; tabSizes[newTab]:=tabsize;

		inc(ntopptr,tabsize);
	end;

begin
	// determine end of SFX definition offet in data table
	ntopptr:=0;
	for nSFX:=0 to 63 do
		if (sfxptr[nSFX]<>$FFFF) and (sfxptr[nSFX]+sfxSizes[nSFX]>ntopptr) then ntopptr:=sfxptr[nSFX]+sfxSizes[nSFX];

	// remove unused TAB definition
	newTAB:=0;
	for nTAB:=0 to 63 do
	begin
		if TABUsage[nTAB]<>-1 then
		begin
			if not TABreindex then newTAB:=TABUsage[nTAB];
			moveTAB(nTAB,newTAB);
			if TABreindex then
			begin
				TABUsage[nTAB]:=newTAB;
				inc(newTAB);
			end;
		end;
	end;

	// change TAB indexes in SONG definition
	songofs:=0;
	repeat
		for track:=0 to 3 do
		begin
			se:=song[songofs];
			if (track=0) and (se and $80<>0) then
			begin
				inc(songofs,4); break;
			end;

			if se and $40=0 then
			begin
				se:=se and $3f;
				song[songofs]:=TABUsage[se];
			end;
			inc(songofs);
		end;
	until songofs=$100;

	topptr:=ntopptr;
end;
