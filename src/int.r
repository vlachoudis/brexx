signal on syntax;
signal on error;
start:do forever;
call write ,">>> ";
parse pull _;
interpret _;
end;
signal start;
syntax: ; error: say "+++ Error" RC":" errortext(RC);
signal start
