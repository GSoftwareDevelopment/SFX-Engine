unit SMMFile;

interface
uses SysUtils, BufFile;

const
	ERR_SOURCE_NOT_EXIST = 'Source file not exist';
	ERR_SMMFiLE_UNEXPECTED_EOF = 'Unexpected end of file';
	ERR_SMMFILE_INCORRECT_TAG = 'Incorrect tag in source file';
	ERR_SMMFILE_NOTETABLE_BAD_ID = 'Bad index of note table definition';
	ERR_SMMFILE_SFX_BAD_ID = 'Bad index of SFX definition';
	ERR_SMMFILE_SFX_MOD_BAD_ID = 'Bad modulator type in SFX definition';
	ERR_SMMFILE_SFX_NOTETABLE_BAD_ID = 'Bad note table of SFX definition';
	ERR_SMMFILE_TAB_BAD_ID = 'Bad index of TAB definition';
	ERR_SMMFILE_INCORRECT_DATA_LENGTH = 'Incorrect data length';
  ERR_SMMFILE_INCORRECT_SONG_DATA_LENGTH = 'Incorrect song data length';

type
	TTag = array[0..4] of byte;
	byteArray = array of byte;

	PSFXStruct = ^TSFXStruct;
	TSFXStruct = array[0..127] of record
		order:byte;
		param:byte;
	end;

	PTABStruct = ^TTABStruct;
	TTABStruct = array[0..127] of record
		order:byte;
		param:byte;
	end;

	PSONGStruct = ^TSOngStruct;
	TSONGStruct = array[0..63,0..3] of byte;

//
//
//

	PData = ^TData;
	TData = object
		name:string[16];
		size:word;
		data:pointer;
		len:byte;
		constructor init(_name:String; var _data; _size:word);
		destructor done;
	end;

	PSFXData = ^TSFXData;
	TSFXData = object(TData)
		mode:^byte;
		note:^byte;
	end;

	PTABData = ^TTABData;
	TTABData = object(TData)
	end;

  TSMMFile = object
		filename:string;
		version: byte;
		songname: string[32];
		tempo: byte;

		notetab:array[0..3] of byteArray;
		SFXModTable:byteArray;
		SFXNoteTable:byteArray;
		song:byteArray;
		SFX:array[0..63] of PSFXData;
		TAB:array[0..63] of PTABData;
		totalSFX,
		totalTAB:byte;

		errmsg:String;

		constructor Init(fn:string);
		procedure load();
		function getSFXDataSize():word;
		function getTABDataSize():word;
		procedure error(msg:string);
		destructor done();

	private
		f:TFile;
		Procedure loadMain();
		Procedure loadNoteTab();
		Procedure loadDefinition(isSFX:boolean; nameLength:byte);
		Procedure loadSONG();
		Function compareTag(Var dstTag:TTag): boolean;
	end;

implementation

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

//
//
//

constructor TData.init();
begin
	name:=_name;
	size:=_size;
	len:=_size div 2;
	getMem(data,size);
	move(_data,data^,size);
end;

destructor TData.done();
begin
	freeMem(data,size);
	name:='';
	size:=0;
	len:=0;
end;

//
//
//

constructor TSMMFile.Init(fn:string);
var
	i,j:byte;

begin
	filename:=fn;
	songname:='';
	version:=$00;
	tempo:=255;

	for i:=0 to 3 do
	begin
		setLength(notetab[i],64);
		for j:=0 to 63 do notetab[i][j]:=0;
	end;
	setLength(song,256);
	for j:=0 to 255 do song[j]:=0;
	setLength(SFXModTable,64);
	setLength(SFXNoteTable,64);

	for i:=0 to 63 do
	begin
		SFX[i]:=nil;
		TAB[i]:=nil;
		SFXModTable[i]:=0;
		SFXNoteTable[i]:=0;
	end;
	totalSFX:=0;
	totalTAB:=0;
	errmsg:='';
end;

Procedure TSMMFile.loadMain();
var
	nameLen:byte;

Begin
  If f.getByte(version) Then exit;

  If f.getByte(nameLen) Then exit;
  If (nameLen=SONGNameLength) Then
    Begin
      If f.getBlock(songname[1],nameLen) Then exit;
			while (nameLen>0) and (songname[nameLen]=#0) do dec(nameLen);
      setLength(songname,nameLen);
    End;
End;

Procedure TSMMFile.loadNoteTab();
Var
  i,noteTabId: byte;

Begin
  If f.getByte(noteTabId) Then exit;
  // read ID
  If noteTabId>maxNoteTables-1 Then Error(ERR_SMMFILE_NOTETABLE_BAD_ID);

  f.getBlock(IOBuf,NOTETABnameLength+64);
  // read data

  If f.IOErr>3 Then exit;

  For i:=0 To 63 Do
    notetab[noteTabId][i]:=IOBuf[NOTETABnameLength+i];
End;

Procedure TSMMFile.loadDefinition(isSFX:boolean; nameLength:byte);
Var
  id,_sfxMode,_sfxNoteTabOfs: byte;
  dataSize: word;
	len:byte;
	name:string[16];

Begin
  If f.getByte(id) Then exit;
  // read ID

  If (isSFX) Then
    Begin
      If id>maxSFXs-1 Then Error(ERR_SMMFILE_SFX_BAD_ID);

      If f.getByte(_sfxMode) Then exit;
      // read SFX Modulation Type
      If _sfxMode>3 Then Error(ERR_SMMFILE_SFX_MOD_BAD_ID);


      If f.getByte(_sfxNoteTabOfs) Then exit;
      // read SFX note table index
      If _sfxNoteTabOfs>3 Then Error(ERR_SMMFILE_SFX_NOTETABLE_BAD_ID);
    End
  Else
    Begin
      If id-64>maxTABs-1 Then Error(ERR_SMMFILE_TAB_BAD_ID);
      id:=id-64;
    End;

  If f.getBlock(dataSize,2) Then exit;
  // read data size
  If dataSize>256+nameLength Then Error(ERR_SMMFILE_INCORRECT_DATA_LENGTH);

  f.getBlock(IOBuf,dataSize);
  // read data
  If dataSize<>f.IOLength Then exit;

	len:=0; while (IOBUF[len]<>0) and (len<nameLength) do inc(len);
	setLength(name,len);
	move(IOBuf[0],name[1],len);

  dec(dataSize,nameLength);
  If (isSFX) Then
	Begin
		writeLn(stdout,id,' ',name,' ',dataSize,' ',_sfxMode,' ',_sfxNoteTabOfs);

		new(SFX[id],init(name,IOBuf[nameLength],dataSize));
		SFXModTable[id]:=_sfxMode;
		SFXNoteTable[id]:=_sfxNoteTabOfs+$40;
		SFX[id].mode:=@SFXModTable[id]; // _sfxMode; // sfxmodes[id]:=_sfxMode;
		SFX[id].note:=@SFXNoteTable[id]; // _sfxNoteTabOfs*$40; // sfxnote[id]:=_sfxNoteTabOfs*$40;
		inc(totalSFX);
	End
  Else
	Begin
		new(TAB[id],init(name,IOBuf[nameLength],dataSize));
		inc(totalTAB);
	End;
End;

Procedure TSMMFile.loadSONG();
Var
  dataSize: word;

Begin
//  if verbose>0 then WriteLn('SONG section:');
  If version=SFXMM_VER1_2 Then
    If f.getByte(tempo) Then exit;

  If f.getBlock(dataSize,2) Then exit;
  // read data size
  If dataSize>256 Then Error(ERR_SMMFILE_INCORRECT_SONG_DATA_LENGTH);

  f.getBlock(song,dataSize);
  // read data
  If f.IOErr>3 Then exit;
End;

Function TSMMFile.compareTag(Var dstTag:TTag): boolean;
Var
  i: byte;

Begin
  i:=0;
  While (i<5) And (IOBUf[i]=dstTag[i]) Do
    i:=i+1;
  result:=(i=5);
End;

procedure TSMMFile.load();
begin
	if not fileExists(filename) then
    error(ERR_SOURCE_NOT_EXIST);

  totalSFX:=0;
  totalTAB:=0;
  f.openFile(fRead,filename);
  While (f.IOErr=0) And (Not f.EOF) Do
	Begin
		// get tag
		If (Not f.getBlock(IOBuf,5)) Then
			Begin
				If compareTag(section_main) Then
					loadMain()
				Else If compareTag(section_NOTE) Then
							loadNoteTab()
				Else If compareTag(section_SFX) Then
							loadDefinition(true,SFXNameLength)
				Else If compareTag(section_TAB) Then
							loadDefinition(false,TABNameLength)
				Else If compareTag(section_SONG) Then
							loadSONG()
				Else
					Error(ERR_SMMFILE_INCORRECT_TAG);
			End
		Else
			Error(ERR_SMMFiLE_UNEXPECTED_EOF)
		// break;
	End;
  If f.IOErr<>0 Then
    Error(ERR_SMMFiLE_UNEXPECTED_EOF);
  f.closeFile();
end;

function TSMMFile.getSFXDataSize():word;
var
	i:byte;

begin
	result:=0;
	for i:=0 to 63 do
		if SFX[i]<>nil then inc(result,SFX[i].size);
end;

function TSMMFile.getTABDataSize():word;
var
	i:byte;

begin
	result:=0;
	for i:=0 to 63 do
		if TAB[i]<>nil then inc(result,TAB[i].size);
end;

procedure TSMMFile.error(msg:string);
begin
	errmsg:=msg;
	writeLn(stdout,'SMMError: ',msg);
	halt;
end;

destructor TSMMFile.done();
var
	i:byte;

begin
	if filename='' then exit;
	for i:=0 to 63 do
	begin
		if SFX[i]<>nil then dispose(SFX[i],done);
		if TAB[i]<>nil then dispose(TAB[i],done);
	end;
end;

End.