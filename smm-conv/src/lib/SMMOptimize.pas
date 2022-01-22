unit SMMOptimize;

interface
uses SMMFile;

function SMMReduceSFX(var SMM:TSMMFile):byte;
function SMMReindexSFX(var SMM:TSMMFile):shortint;
function SMMReduceTAB(var SMM:TSMMFile):byte;
function SMMReindexTAB(var SMM:TSMMFile):shortint;
function SMMReduceNoteTab(var SMM:TSMMFile):byte;

implementation

function SMMReduceSFX(var SMM:TSMMFile):byte;
var
	nSFX:byte;

	function checkSFXinTABs(nSFX:byte):boolean;
	var
		nTAB,tabrow:byte;
		rd2:byte;
		data:PTABStruct;
		TAB:TTABData;

	begin
		result:=false;
		for nTAB:=0 to 63 do
		begin
			if SMM.TAB[nTAB]=nil then continue; // skip not defined TAB;
			TAB:=SMM.TAB[nTAB]^;
			data:=PTABStruct(TAB.data);
			for tabrow:=0 to TAB.len-1 do
			begin
				rd2:=data^[tabrow].param;	// get SFX ID
				if rd2 and $80<>0 then continue;
				if rd2 and $3f=nSFX then exit(true); // found, exit;
			end;
		end;
	end;

begin
	result:=0;
	for nSFX:=0 to 63 do
	begin
		if SMM.SFX[nSFX]=nil then continue; // skip not defined SFX
		if checkSFXinTABs(nSFX) then
			inc(result)
		else
		begin
			dispose(SMM.SFX[nSFX],done); // delete unused SFX
			SMM.SFX[nSFX]:=nil;
		end;
	end;
end;

function SMMReindexSFX(var SMM:TSMMFile):shortint;
var
	nSFX,newSFX:shortint;
	nTAB:Byte;
	SFXUsage:array[0..63] of shortint;

	tabrow:byte;
	rd1,rd2:byte;
	TAB:TTABData;
	data:PTABStruct;

begin
	result:=0;
	for nSFX:=0 to 63 do SFXUsage[nSFX]:=-1;

	newSFX:=0;
	for nSFX:=0 to 63 do
	begin
		if SMM.SFX[nSFX]=nil then continue;

		if newSFX<>nSFX then
		begin
			SMM.SFX[newSFX]:=SMM.SFX[nSFX];
			SMM.SFXModTable[newSFX]:=SMM.SFXModTable[nSFX];
			SMM.SFXNoteTable[newSFX]:=SMM.SFXNoteTable[nSFX];
			SMM.SFX[nSFX]:=nil;
			SMM.SFXModTable[nSFX]:=0;
			SMM.SFXNoteTable[nSFX]:=0;
		end;

		SFXUsage[nSFX]:=newSFX;
		inc(newSFX);
	end;

	for nTAB:=0 to 63 do
	begin
		if SMM.TAB[nTAB]=nil then continue; // skip not defined TAB
		TAB:=SMM.TAB[nTAB]^;
		data:=PTABStruct(TAB.Data);

		for tabrow:=0 to TAB.len-1 do
		begin
			rd1:=data^[tabrow].order;
			rd2:=data^[tabrow].param;
			if rd2 and $80=0 then
			begin
				nSFX:=SFXUsage[rd2 and $3f];
				if nSFX=-1 then exit(-1); // HaltError(ERR_SFX_REINDEX);
				rd2:=(rd2 and $c0) or nSFX;
			end;
			data^[tabrow].order:=rd1;
			data^[tabrow].param:=rd2;
		end;
	end;
	result:=1;
end;

function SMMReduceTAB(var SMM:TSMMFile):byte;
var
	nTab:byte;

	function checkTABinSONG(nTAB:byte):boolean;
	var
		songLine:byte;
		song:PSONGStruct;

		se,track:byte;

	begin
		result:=false;
		song:=@SMM.song;
		for songLine:=0 to 63 do
		begin
			if song^[songLine][0] and $80<>0 then continue; // skip order line in song
			for track:=0 to 3 do
			begin
				se:=song^[songLine][track];
				if se and $40=0 then
					if se and $3f=nTAB then exit(true); // found, exit;
			end;
		end;
	end;

begin
	result:=0;
	for nTAB:=0 to 63 do
	begin
		if SMM.TAB[nTAB]=nil then continue; // skip undefined TAB
		if checkTABinSONG(nTAB) then
			inc(result)
		else
		begin
			dispose(SMM.TAB[nTAB],done);
			SMM.TAB[nTAB]:=nil;
		end;
	end;
end;

function SMMReindexTAB(var SMM:TSMMFile):shortint;
var
	nTAB,newTAB:byte;
	songLine:byte;
	song:PSONGStruct;
	se,track:byte;
	TABUsage:array[0..63] of shortint;

begin
	result:=0;
	for nTAB:=0 to 63 do TABUsage[nTAB]:=-1;

	newTAB:=0;
	for nTAB:=0 to 63 do
	begin
		if SMM.TAB[nTAB]=nil then continue; // skip undefined TAB
		if (nTAB<>newTab) then
		begin
			SMM.TAB[newTAB]:=SMM.TAB[nTAB];
			SMM.TAB[nTAB]:=nil;
		end;

		TABUsage[nTAB]:=newTAB;
		inc(newTAB);
	end;

	song:=@SMM.song;
	// change TAB indexes in SONG definition
	for songLine:=0 to 63 do
	begin
		if (song^[songLine][0] and $80<>0) then continue; // skip song order line

		for track:=0 to 3 do
		begin
			se:=song^[songLine][track];

			if se and $40=0 then
			begin
				se:=se and $3f;
				if TABUsage[se]=-1 then exit(-1);
				song^[songLine][track]:=TABUsage[se];
			end;
		end;
	end;
	result:=1;
end;

function SMMReduceNoteTab(var SMM:TSMMFile):byte;
var
	i,id:byte;
	ntab:array[1..4] of boolean;

begin
	for i:=1 to 4 do ntab[i]:=false;
	for i:=0 to 63 do
	begin
		id:=SMM.SFXNoteTable[i];
		case id of
			$00:ntab[1]:=true;
			$40:ntab[2]:=true;
			$80:ntab[3]:=true;
			$C0:ntab[4]:=true;
		end;
	end;
	for i:=1 to 4 do
		if not ntab[i] then
		begin
			freeMem(SMM.noteTable[i],64);
			SMM.noteTable[i]:=nil;
		end;
end;

end.