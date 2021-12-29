Procedure loadSMM(fn:String);

Var
  f: TFile;
  version: byte;
  nameLen: byte;

Procedure loadMain();

Var
  songname: string[32];

Begin
  If f.getByte(version) Then exit;

  If f.getByte(nameLen) Then exit;
  If (nameLen=SONGNameLength) Then
    Begin
      If f.getBlock(songname[1],nameLen) Then exit;
      songname[0] := char(nameLen);
    End;
  //      setName(songname);
End;

Procedure loadNoteTab();

Var
  i,noteTabId: byte;

Begin
  If f.getByte(noteTabId) Then exit;
  // read ID
  If noteTabId>maxNoteTables-1 Then haltError('Bad index of note table definition');

  f.getBlock(IOBuf,NOTETABnameLength+64);
  // read data

  If f.IOErr>3 Then exit;

  For i:=0 To 63 Do
    notetabs[noteTabId*64+i] := IOBuf[NOTETABnameLength+i];
End;

Procedure loadDefinition(isSFX:boolean; nameLength:byte);

Var
  id,_sfxMode,_sfxNoteTabOfs: byte;
  dataSize: word;
	len:byte;

Begin
  If f.getByte(id) Then exit;
  // read ID

  If (isSFX) Then
    Begin
      If id>maxSFXs-1 Then haltError('Bad index of SFX definition');

      If f.getByte(_sfxMode) Then exit;
      // read SFX Modulation Type
      If _sfxMode>3 Then haltError('');

      sfxmodes[id] := _sfxMode;

      If f.getByte(_sfxNoteTabOfs) Then exit;
      // read SFX note table index
      If _sfxNoteTabOfs>3 Then haltError('Bad note table of SFX definition');
      sfxnote[id] := _sfxNoteTabOfs*$40;
      inc(totalSFX);
    End
  Else
    Begin
      If id-64>maxTABs-1 Then haltError('');
      id := id-64;

      inc(totalTAB);
    End;

  If f.getBlock(dataSize,2) Then exit;
  // read data size
  If dataSize>256+nameLength Then haltError('Incorrect length of SFX/TAB data');

  f.getBlock(IOBuf,dataSize);
  // read data
  If dataSize<>f.IOLength Then exit;

	len:=0; while (IOBUF[len]<>0) and (len<nameLength) do inc(len);

  dec(dataSize,nameLength);
  If (isSFX) Then
    Begin
      sfxnames[id][0] := char(len);
      move(IOBuf[0],sfxnames[id][1],len);
      sfxptr[id] := topPtr;
      sfxSizes[id] := dataSize;
    End
  Else
    Begin
      tabnames[id][0] := char(len);
      move(IOBuf[0],tabnames[id][1],len);
      tabptr[id] := topPtr;
      tabSizes[id] := dataSize;
    End;
  move(IOBuf[nameLength],data[topPtr],dataSize);
  inc(topPtr,dataSize);
End;

Procedure loadSONG();

Var
  dataSize: word;
  tempo: byte;

Begin
//  if verbose>0 then WriteLn('SONG section:');
  If version=SFXMM_VER1_2 Then
    If f.getByte(tempo) Then exit;

  If f.getBlock(dataSize,2) Then exit;
  // read data size
  If dataSize>256 Then haltError('Incorrect song data length');

  f.getBlock(song,dataSize);
  // read data
  If f.IOErr>3 Then exit;
End;

Function compareTag(Var dstTag:TTag): boolean;

Var
  i: byte;

Begin
  i := 0;
  While (i<5) And (IOBUf[i]=dstTag[i]) Do
    i := i+1;
  result := (i=5);
End;

Begin
	// TODO: Sprawdź najpierw, czy plik źródłowy istnieje!
	// TODO: Przerwij z błędem, jeżeli nie istnieje!

	if verbose>0 then
	begin
		writeLn(stdout);
		write(stdout,'Reading `'+fn+'` file...');
	end;
  totalSFX := 0;
  totalTAB := 0;
  f.openFile(fRead,fn);
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
            haltError('Incorrect TAG in source file');
        End
      Else
        haltError('Unexpected end of file');
      // break;
    End;
  If f.IOErr<>0 Then
    haltError('Unexpected end of file');
  f.closeFile();

	if verbose>0 then
	begin
		writeLn(stdout);
		writeLn(stdout,totalSFX,' SFX(s) declared');
		writeLn(stdout,totalTAB,' TAB(s) declared');
		writeLn(stdout,'Data (SFXs & TABs) size: ',topPtr,' byte(s)');
		writeLn(stdout);
	end;
End;
