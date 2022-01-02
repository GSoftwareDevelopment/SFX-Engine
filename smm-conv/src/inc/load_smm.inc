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
  If noteTabId>maxNoteTables-1 Then haltError(ERR_SMMFILE_NOTETABLE_BAD_ID);

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
      If id>maxSFXs-1 Then haltError(ERR_SMMFILE_SFX_BAD_ID);

      If f.getByte(_sfxMode) Then exit;
      // read SFX Modulation Type
      If _sfxMode>3 Then haltError(ERR_SMMFILE_SFX_MOD_BAD_ID);

      sfxmodes[id] := _sfxMode;

      If f.getByte(_sfxNoteTabOfs) Then exit;
      // read SFX note table index
      If _sfxNoteTabOfs>3 Then haltError(ERR_SMMFILE_SFX_NOTETABLE_BAD_ID);
      sfxnote[id] := _sfxNoteTabOfs*$40;
      inc(totalSFX);
    End
  Else
    Begin
      If id-64>maxTABs-1 Then haltError(ERR_SMMFILE_TAB_BAD_ID);
      id := id-64;

      inc(totalTAB);
    End;

  If f.getBlock(dataSize,2) Then exit;
  // read data size
  If dataSize>256+nameLength Then haltError(ERR_SMMFILE_INCORRECT_DATA_LENGTH);

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
			sfxUsage[id] := id;
    End
  Else
    Begin
      tabnames[id][0] := char(len);
      move(IOBuf[0],tabnames[id][1],len);
      tabptr[id] := topPtr;
      tabSizes[id] := dataSize;
			tabUsage[id] := id;
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
  If dataSize>256 Then haltError(ERR_SMMFILE_INCORRECT_SONG_DATA_LENGTH);

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
	if verbose>0 then
	begin
		EOLStdOut;
		writeStdOut(STDOUT_READING_SMM_FILE,[fn]);
	end;
	if not fileExists(fn) then
    haltError(ERR_SOURCE_NOT_EXIST);

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
					haltError(ERR_SMMFILE_INCORRECT_TAG);
			End
		Else
			haltError(ERR_SMMFiLE_UNEXPECTED_EOF);
		// break;
	End;
  If f.IOErr<>0 Then
    haltError(ERR_SMMFiLE_UNEXPECTED_EOF);
  f.closeFile();

	if verbose>0 then
	begin
		EOLStdOut(2);
		writeLnStdOut(STDOUT_SUMMARY_SFX,[totalSFX]);
		writeLnStdOut(STDOUT_SUMMARY_TAB,[totalTAB]);
		writeLnStdOut(STDOUT_SUMMARY_DATA_SIZE,[topPtr]);
		EOLStdOut;
	end;
End;
