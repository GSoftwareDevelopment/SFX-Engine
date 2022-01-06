unit BufFile;

interface
const
	fRead    = 1;
	fWrite   = 2;

var
	IOBuf:array[0..511] of byte;

type
	TFile = Object
		f:file;
		IOErr:byte;
		IOLength:word;
		constructor openFile(mode:byte; fn:string);
		function getBlock(var dest; size:word):boolean;
		function getByte(var dest):boolean;
		function EOF():boolean;
		destructor closeFile();
	end;

implementation

constructor TFile.openFile(mode:byte; fn:string);
begin
	assign(f,fn);
	case mode of
		fRead: reset(f,1);
		fWrite: rewrite(f,1);
	end;
	IOErr:=IOResult;
	if IOErr<>0 then
	begin
		writeLn(stderr,'I/O Error #',IOErr); Halt(-1);
	end;
end;

function TFile.getBlock(var dest; size:word):boolean;
begin
	blockRead(f,dest,size,IOLength);
	IOErr:=IOResult;
	result:=(IOErr<>0) or (IOLength<>size);
end;

function TFile.getByte(var dest):boolean;
begin
	result:=getBlock(dest,1);
end;

function TFile.EOF():boolean;
begin
	result:=system.eof(f);
end;

destructor TFile.closeFile();
begin
	close(f);
end;

(*
procedure push2IOBuf(data:pointer; size:word);
begin
	move(data,IOBuf[IOOfs],size); inc(IOOfs,size);
end;

function putBlock(var dest; size:word):boolean;
begin
	blockWrite(f,dest,size,IOLength);
	IOErr:=IOResult;
	result:=IOResult=0;
end;

function putIOBuf():boolean;
begin
	result:=putBlock(IOBuf,IOOfs);
end;

function IOBufGetByte:byte;
begin
	result:=IOBuf[IOOfs]; inc(IOOfs);
end;

procedure IOBufGetBlock(dest:pointer; size:byte);
begin
	move(IOBuf[IOOfs],dest,size); inc(IOOfs,size);
end;
*)

end.