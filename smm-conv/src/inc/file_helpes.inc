var
	f,fconf,fres:text;
	writeConf,
	writeRes:boolean;

procedure EOLStdOut(count:byte = 1);
begin
	if verbose=0 then exit;
	while count>0 do
	begin
		writeLn(stdout); dec(count);
	end;
end;

procedure writeStdOut(const s:string; const prm:array of const);
begin
	if verbose>0 then
		write(stdout,format(s,prm));
end;

procedure writeLnStdOut(const s:string; const prm:array of const);
begin
	if verbose>0 then
		writeLn(stdout,format(s,prm));
end;

//
//
//

procedure _EOL(var f:text; count:byte = 1);
begin
	while count>0 do
	begin
		writeLn(f); dec(count);
	end;
end;

procedure _write(var f:text; s:string; isW:boolean);
begin
	if isW then
		write(f,s);
{$IFDEF SLOW}
	sleep(100);
{$ENDIF}
end;

procedure _writeLn(var f:text; s:string; isW:boolean);
begin
	if isW then
		writeLn(f,s);
{$IFDEF SLOW}
	sleep(100);
{$ENDIF}
end;