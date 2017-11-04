function s=DecodeBROSError(x)
% This function returns a multiline string-array explaining, in understandable text, the
% DriveError values.
% The input argument x can be either:
% <number>: This number is decoded. 
% <block id> (as obtained by `gcb`): The data from this display block is used
% [<number>,<number>]: Assumes that these TWO numbers
%          represent erorr codes of BROS1 and BROS2
%% Get the DriveError values
if isscalar(x)
    valuesToDecode = x;
    brosNames = {'BROS'};
elseif ischar(x) % block identifier
    rto = get_param(x, 'RunTimeObject');
    if isempty(rto)
        s = 'Error: no data in block (is the model running and connected?)';
        return 
    end;
    p = get(rto.InputPort(1));
    valuesToDecode = p.Data;
    brosNames = {'BROS1','BROS2'};
elseif isvector(x) 
    if numel(x)~=2
        s='Error: vector should have 2 elements';
    end;
    valuesToDecode = x;
    brosNames = {'BROS1','BROS2'};
else
    s = 'Error: Unknown format of argument passed to DecodeDriveError';
    return
end;

%% Decode the value

if exist('BROSErrorDescriptions.txt','file')
    errorDescriptions = readErrorMsgFile('BROSErrorDescriptions.txt');
else
    % error description file does not exist, write error description file
    errorDescriptions = writeErrorMsgFile('BROSErrorDescriptions.txt');
end

s='';
for i=1:length(valuesToDecode)
    v = valuesToDecode(i);
    n = brosNames{i};
    
    if valuesToDecode(i)<=-1
        inUseStr='(not in use)';
    else
        inUseStr='';
    end;
    s=strvcat(s,sprintf('%s: %s %s',brosNames{i},hex(valuesToDecode(i)),inUseStr));
    anyError = 0;
    
    for j = 1:length(errorDescriptions)
        vv = int32(abs(v));
        n = int32(2^(j-1));
        if bitand(vv,n)
%             s=strvcat(s,sprintf('   %s %s', hex(n), errorDescriptions{j}));
            s = char(s,sprintf('   %i %s', j, errorDescriptions{j}));
            anyError = 1;
        end;
    end;
    
    s=char(s,'');
    
end
end


function s=hex(x)
% Custom hex function which allows negative numbers and assumes it is an
% int32
if x<0
    x=2^32+x;
end;
s=['0x' dec2hex(x,8)];
end

function emsgs = writeErrorMsgFile(filename)
% error messages
errorSingleActuator = {...
    'Joint position guard';...
    'Joint velocity guard';...
    'Joint torque guard';...
    'Transmission slip';...
    'Drive error';...
    'Emergency button pressed'; ...
    'Motor undervoltage'; ...
    'Motor overvoltage'; ...
    'Drive overtemperature'; ...
    'I2t error'; ...
    'Overcurrent'; ...
    'Following/control error'; ...
    'Short circuit error';...
    'Other drive error (see MotionErrorRegister)'
    };
emsgs = [...
    strcat('Act1.',errorSingleActuator);...
    strcat('Act2.',errorSingleActuator);...
    'Endpoint position guard';...
    'Internal angle guard';...
    'F/T sensor range exceeded'
   ];

% write error messages to file (so we use the same in C++)
fid = fopen(filename,'w');
if fid > 0
    for ii = 1:length(emsgs)
        fprintf(fid,'%s',emsgs{ii});
        if ii < length(emsgs)
            fprintf(fid,'\n');
        end
    end
    fclose(fid);
end

end

function emsgs = readErrorMsgFile(filename)   
% Read columns of data as text:
% For more information, see the TEXTSCAN documentation.
formatSpec = '%s%[^\n\r]';

% Open the text file.
fileID = fopen(filename,'r');

% Read columns of data according to the format.
% This call is based on the structure of the file used to generate this
% code. If an error occurs for a different file, try regenerating the code
% from the Import Tool.
dataArray = textscan(fileID, formatSpec, 'Delimiter', '', 'MultipleDelimsAsOne', true,  'ReturnOnError', false);

fclose(fileID);

% Convert the contents of columns containing numeric text to numbers.
% Replace non-numeric text with NaN.
raw = repmat({''},length(dataArray{1}),length(dataArray)-1);
for col=1:length(dataArray)-1
    raw(1:length(dataArray{col}),col) = dataArray{col};
end

emsgs = raw(:,1);
end